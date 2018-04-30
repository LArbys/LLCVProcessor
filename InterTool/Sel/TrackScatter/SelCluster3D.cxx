#ifndef __SELCLUSTER3D_CXX__
#define __SELCLUSTER3D_CXX__

#include "SelCluster3D.h"

#include "InterTool_Util/InterImageUtils.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "Object3D.h"

#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <array>

namespace llcv {

  void SelCluster3D::Configure (const larcv::PSet &pset) {

    set_verbosity((msg::Level_t)pset.get<int>("Verbosity",2));
    LLCV_DEBUG() << "start" << std::endl;
    
    _cropx = 400;
    _cropy = 400;

    _twatch.Stop();

    LLCV_DEBUG() << "end" << std::endl;
  }

  void SelCluster3D::Initialize() {
    LLCV_DEBUG() << "start" << std::endl;

    _outtree = new TTree("SelCluster3D","");
    AttachRSEV(_outtree);

    _outtree->Branch("shower_x_v", &_shower_x_v);
    _outtree->Branch("shower_y_v", &_shower_y_v);
    _outtree->Branch("shower_z_v", &_shower_z_v);

    LLCV_DEBUG() << "end" << std::endl;
  }

  double SelCluster3D::Select() {
    LLCV_DEBUG() << "start" << std::endl;

    LLCV_DEBUG() << "(RSEV)=("<< Run() << "," << SubRun() << "," << Event() << "," << VertexID() << ")" << std::endl;    
    LLCV_DEBUG() << "VTX=(" 
		 << Data().Vertex()->X() << "," 
		 << Data().Vertex()->Y() << "," 
		 << Data().Vertex()->Z() << ")" << std::endl;

    int isnue = Tree().Scalar<int>("isnue");
    LLCV_DEBUG() << "Is this nue?: " <<  isnue << std::endl;
    if (isnue == 0)  {
      LLCV_DEBUG() << "skip" << std::endl;
      return 0.0;
    }
    
    //
    // get the image centered around the vertex
    //
    
    auto mat_v  = Image().Image<cv::Mat>(kImageADC,_cropx,_cropy);
    auto meta_v = Image().Image<larocv::ImageMeta>(kImageADC,_cropx,_cropy);
    auto img_v  = Image().Image<larcv::Image2D>(kImageADC,_cropx,_cropy);
    auto dead_v = Image().Image<cv::Mat>(kImageDead,_cropx,_cropy);

    for(auto const& meta : meta_v)
      _PixelScan3D.SetPlaneInfo(*meta);

    std::vector<cv::Mat> thresh_mat_v;
    std::vector<cv::Mat> mat3d_v;
    mat3d_v.reserve(3);

    thresh_mat_v.reserve(3);

    for(size_t plane=0; plane<3; ++plane) {
      const auto mat = mat_v[plane];
      auto img = larocv::Threshold(*mat,10,255);
      auto img3d = As8UC3(img);
      thresh_mat_v.emplace_back(std::move(img));      
      mat3d_v.emplace_back(std::move(img3d));
    }
    
    larocv::data::Vertex3D vtx3d;
    vtx3d.x = Data().Vertex()->X();
    vtx3d.y = Data().Vertex()->Y();
    vtx3d.z = Data().Vertex()->Z();
  
    std::array<cv::Mat,3> timg_v;
    
    for(size_t plane=0; plane<3; ++plane) {
      const auto& thresh_mat = thresh_mat_v[plane];
      timg_v[plane] = thresh_mat;
    }

    const auto& shr_v = Data().Showers();
    
    for(size_t shrid=0; shrid < shr_v.size(); ++shrid) {
      const auto& shr = (*shr_v[shrid]);

      auto theta_phi = ShowerAngle(shr);
      auto length    = shr.Length();

      float pi8 = 3.14159 / 8;
      float pi4 = 3.14159 / 4;
      
      float rad_min = 0.5;
      float rad_max = 50;
      float rad_step = 0.5;
      
      float theta_min = theta_phi.first - pi8;
      float theta_max = theta_phi.first + pi8;
      
      float phi_min = theta_phi.second - pi4;
      float phi_max = theta_phi.second + pi4;
      
      ResizeOutput(0);
      
      LLCV_DEBUG() << "Scanning Spheres" << std::endl;
      LLCV_DEBUG() << "rad_min=" << rad_min << " rad_max=" << rad_max << " rad_step=" << rad_step << std::endl;
      LLCV_DEBUG() << "theta_min=" << theta_min << " theta_max=" << theta_max << std::endl;
      LLCV_DEBUG() << "phi_min=" << phi_min << " phi_max=" << phi_max << std::endl;

      //
      // Build 3D object
      //
      _twatch.Start();
      
      _PixelScan3D.Reconfigure(rad_min, rad_max, rad_step,
			       theta_min, theta_max,
			       phi_min, phi_max);
            
      std::vector<larocv::data::Vertex3D> reg_v;
      
      reg_v = _PixelScan3D.SphereScan3D(timg_v,dead_v,vtx3d);
      
      _twatch.Stop();
      
      LLCV_DEBUG() << reg_v.size() << " spheres scanned in " 
		   << std::setprecision(6) << _twatch.RealTime() << "s" << std::endl;
    
      if (!reg_v.empty()) {
	LLCV_DEBUG() << "Objectify" << std::endl;
	_twatch.Start();
	Object3D obj(vtx3d,reg_v);
	_twatch.Stop();
	LLCV_DEBUG() << "...objectified in " << std::setprecision(6) << _twatch.RealTime() << "s" << std::endl;
      
	for(size_t pid=0; pid < obj.Points().size(); ++pid) {
	  const auto& pt = obj.Points()[pid];
	  _shower_x_v.push_back(pt.x);
	  _shower_y_v.push_back(pt.y);
	  _shower_z_v.push_back(pt.z);
	  for(size_t plane=0; plane<3; ++plane) {
	    auto& mat3d = mat3d_v[plane];
	    int px_x = pt.vtx2d_v[plane].pt.x;
	    int px_y = pt.vtx2d_v[plane].pt.y;
	    if (px_x < 0) continue;
	    if (px_y < 0) continue;
	    if (px_x >= mat3d.rows) continue;
	    if (px_y >= mat3d.cols) continue;
	    mat3d.at<cv::Vec3b>(px_y,px_x) = {255,255,0};
	  } // end plane

	} // end sphere point
      } // end empty obj

    } // end shower
    
    LLCV_DEBUG() << "done!" << std::endl;
    
    for(size_t plane=0; plane<3; ++plane) {
      auto& img3d = mat3d_v[plane];
      std::stringstream ss; 
      ss << "cpng/plane_img_" << Run() << "_" << SubRun() << "_" << Event() << "_" << VertexID() << "_" << plane << ".png";
      cv::imwrite(ss.str(),img3d);
    }
    
    _outtree->Fill();
    
    LLCV_DEBUG() << "end" << std::endl;
    return 0.0;
  }
  
  std::pair<float,float> SelCluster3D::ShowerAngle(const larlite::shower& shower) {
    
    std::pair<float,float> res;
    
    const auto& dir = shower.Direction();

    res.first  = std::acos( dir.Z() / dir.Mag());
    res.second = std::atan2(dir.X(),dir.Y()); // wtf
    
    return res;
  }


  


  void SelCluster3D::Finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    _outtree->Write();
    LLCV_DEBUG() << "end" << std::endl;
  }


  void SelCluster3D::ResizeOutput(size_t sz) {
    _shower_x_v.clear();
    _shower_y_v.clear();
    _shower_z_v.clear();

  }


}


#endif

