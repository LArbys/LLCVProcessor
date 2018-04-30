#ifndef __SELTRIANGLESTUDY_CXX__
#define __SELTRIANGLESTUDY_CXX__

#include "SelTriangleStudy.h"

#include "Triangle.h"


#include "InterTool_Util/InterImageUtils.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

#include <iomanip>
#include <sstream>
#include <cstdlib>

namespace llcv {

  void SelTriangleStudy::Configure(const larcv::PSet &pset) {

    set_verbosity((msg::Level_t)pset.get<int>("Verbosity",2));
    LLCV_DEBUG() << "start" << std::endl;
    
    _cropx = 400;
    _cropy = 400;

    _twatch.Stop();

    LLCV_DEBUG() << "end" << std::endl;
  }

  void SelTriangleStudy::Initialize() {
    LLCV_DEBUG() << "start" << std::endl;

    _outtree = new TTree("SelTriangleStudy","");
    AttachRSEV(_outtree);

    _outtree->Branch("shower_x_v", &_shower_x_v);
    _outtree->Branch("shower_y_v", &_shower_y_v);
    _outtree->Branch("shower_z_v", &_shower_z_v);

    LLCV_DEBUG() << "end" << std::endl;
  }

  double SelTriangleStudy::Select() {
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

    for(const auto& meta : meta_v)
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
    
    //
    // Project the vertex onto the plane
    //
    larocv::GEO2D_Contour_t vertex_ctor;
    vertex_ctor.resize(3);

    for(size_t plane=0; plane<3; ++plane) {
      int px_x = kINVALID_INT;
      int px_y = kINVALID_INT;

      ProjectMat(img_v[plane]->meta(),
    		 Data().Vertex()->X(),Data().Vertex()->Y(),Data().Vertex()->Z(),
    		 px_x, px_y);

      vertex_ctor[plane] = cv::Point_<int>(px_y,px_x);
    }


    std::array<cv::Mat,3> timg_v;
    for(size_t plane=0; plane<3; ++plane) {
      const auto& thresh_mat = thresh_mat_v[plane];
      timg_v[plane] = thresh_mat;
      //timg_v[plane] = larocv::BlankImage(thresh_mat,255);
    }
    

    const auto& shr_v = Data().Showers();
    
    for(size_t shrid=0; shrid < shr_v.size(); ++shrid) {
      const auto& shr = (*shr_v[shrid]);
      
      auto theta_phi = ShowerAngle(shr);
      
      float pi8 = 3.14159 / 8;
      float pi4 = 3.14159 / 4;
      
      float rad_min  = 0.5;
      float rad_max  = 50;
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
    
      if (reg_v.empty()) continue;

      std::array<cv::Mat,3> timg3d_v;
      for(size_t plane=0; plane<3; ++plane)
	timg3d_v[plane] = larocv::BlankImage(timg_v[plane],0);

      
      for(size_t pid=0; pid < reg_v.size(); ++pid) {
	const auto& pt = reg_v[pid];
	for(size_t plane=0; plane<3; ++plane) {
	  auto& mat3d = mat3d_v[plane];
	  int px_x = pt.vtx2d_v[plane].pt.x;
	  int px_y = pt.vtx2d_v[plane].pt.y;
	  if (px_x < 0) continue;
	  if (px_y < 0) continue;
	  if (px_x >= mat3d.rows) continue;
	  if (px_y >= mat3d.cols) continue;
	  mat3d.at<cv::Vec3b>(px_y,px_x) = {255,255,0};
	  timg3d_v[plane].at<uchar>(px_y,px_x) = (uchar)255;
	} // end plane
      } // end sphere point


      // Fill
      FillNeighbors(timg_v,timg3d_v);
      
      // Cluster
      for(size_t plane=0; plane<3; ++plane) {
	const auto& timg3d = timg3d_v[plane];
	auto ctor_v = larocv::FindContours(timg3d);

	LLCV_DEBUG() << "@plane=" << plane << " found " << ctor_v.size() << " ctors" << std::endl;

	const auto vertex_pt = vertex_ctor[plane];

	// get the contour closest to the vertex
	larocv::GEO2D_Contour_t* ctor_ptr = nullptr;
	float dist = kINVALID_FLOAT;
	for(auto& ctor : ctor_v)  {
	  LLCV_DEBUG() << "ctor sz=" << ctor.size() << " vertex_pt=" << vertex_pt << std::endl;

	  auto dist_tmp = larocv::Pt2PtDistance(vertex_pt,ctor);

	  LLCV_DEBUG() << "dist=" << dist << " dist_tmp=" << dist_tmp << std::endl;

	  if (dist_tmp < dist) {
	    dist = dist_tmp;
	    ctor_ptr = &ctor;
	  }
	}
	
	if (!ctor_ptr) throw llcv_err("No contour found?");
	LLCV_DEBUG() << "Close contour @" << ctor_ptr << " sz=" << ctor_ptr->size() << " dist=" << dist << std::endl;
	
	Triangle tri(*ctor_ptr);
	
      }
      
      for(size_t plane=0; plane<3; ++plane) {
	std::stringstream ss;
	auto& img = timg3d_v[plane];
	ss << "cpng/shr_" << shrid << "_img_" << Run() << "_" << SubRun() << "_" << Event() << "_" << VertexID() << "_" << plane << ".png";
	cv::imwrite(ss.str(),img);
      }
      

    } // end shower
    
    LLCV_DEBUG() << "done!" << std::endl;
    
    //
    // Draw the pixel clusters
    //
    std::vector<larocv::GEO2D_ContourArray_t> ctor_vv;
    ctor_vv.resize(3);

    const auto& par_vv = Data().Particles();
    LLCV_DEBUG() << "n particles=" << par_vv.size() << std::endl;
    for(size_t parid=0; parid < par_vv.size(); ++parid) {
      LLCV_DEBUG() << "@parid=" << parid << std::endl;
      const auto& par_v = par_vv[parid];
      for(size_t plane=0; plane<3; ++plane) {
	LLCV_DEBUG() << "@plane=" << plane << std::endl;
	const auto& par = par_v[plane];
	if (par.empty()) continue;
	const auto meta   = meta_v[plane];
	auto ctor = AsContour(par,*meta);
	auto& ctor_v = ctor_vv[plane];
	LLCV_DEBUG() << "saved ctor sz=" << ctor.size() << std::endl;
	ctor_v.emplace_back(std::move(ctor));
      }
    }
    
    for(size_t plane=0; plane<3; ++plane) {
      auto& img3d = mat3d_v[plane];
      const auto& ctor_v = ctor_vv[plane];
      for(size_t cid=0; cid<ctor_v.size(); ++cid) {
	LLCV_DEBUG() << "draw cid=" << cid << " sz=" << ctor_v[cid].size() << std::endl;
	cv::Scalar color(std::rand() % 255,std::rand() % 255,std::rand() % 255);
	cv::drawContours(img3d,ctor_v,cid,color);
      }
    }

    //
    // Write the image
    //
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
  
  std::pair<float,float> SelTriangleStudy::ShowerAngle(const larlite::shower& shower) {
    
    std::pair<float,float> res;
    
    const auto& dir = shower.Direction();

    res.first  = std::acos( dir.Z() / dir.Mag());
    res.second = std::atan2(dir.X(),dir.Y()); // wtf
    
    return res;
  }

  
  void SelTriangleStudy::FillNeighbors(const std::array<cv::Mat,3>& parent_v,
				   std::array<cv::Mat,3>& child_v) const {
    
    for(size_t plane=0; plane<3; ++plane) {
      const auto& parent = parent_v[plane];
      auto& child = child_v[plane];
      
      auto pts_v = larocv::FindNonZero(child);
      
      for(const auto& pt : pts_v) {
	auto spt_v = Neighbors(pt,child);
	for(const auto& spt : spt_v) {
	  uint ret = (uint)parent.at<uchar>(spt.x,spt.y);
	  if (ret==0) continue;
	  child.at<uchar>(spt.x,spt.y) = (uchar)255;
	} // end neighbors
      } // end 3D point
      
    } // end plane
    
  }
  
  larocv::GEO2D_Contour_t SelTriangleStudy::Neighbors(const geo2d::Vector<int>& pt, const cv::Mat& mat) const {
    larocv::GEO2D_Contour_t res;
    res.reserve(9);
    
    int row = pt.y;
    int col = pt.x;

    static std::array<int, 3> row_v;
    static std::array<int, 3> col_v;

    row_v[0] = row;
    row_v[1] = row+1;
    row_v[2] = row-1;
    col_v[0] = col;
    col_v[1] = col+1;
    col_v[2] = col-1;
    
    for(size_t rid=0; rid<3; ++rid) {
      for(size_t cid=0; cid<3; ++cid) {
	if (row_v[rid] < 0 or row_v[cid]>mat.rows) continue;
	if (col_v[cid] < 0 or col_v[cid]>mat.cols) continue;
	res.emplace_back(row_v[rid],col_v[cid]);
      }
    }
    
    return res;
  }


  void SelTriangleStudy::Finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    _outtree->Write();
    LLCV_DEBUG() << "end" << std::endl;
  }


  void SelTriangleStudy::ResizeOutput(size_t sz) {
    _shower_x_v.clear();
    _shower_y_v.clear();
    _shower_z_v.clear();

  }


}


#endif

