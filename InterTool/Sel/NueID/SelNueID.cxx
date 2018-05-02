#ifndef __SELNUEID_CXX__
#define __SELNUEID_CXX__

#include "SelNueID.h"

#include "InterTool_Util/InterImageUtils.h"

#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

#include <iomanip>
#include <sstream>
#include <cstdlib>

namespace llcv {

  void SelNueID::Configure (const larcv::PSet &pset) {
    set_verbosity((msg::Level_t)pset.get<int>("Verbosity",2));
    LLCV_DEBUG() << "start" << std::endl;
    _cropx = 400;
    _cropy = 400;

    _twatch.Stop();

    LLCV_DEBUG() << "end" << std::endl;
  }

  void SelNueID::Initialize() {
    _outtree = new TTree("SelNueID","");
    AttachRSEV(_outtree);
    
    _outtree->Branch("vertex_x", &_vertex_x, "vertex_x/F");
    _outtree->Branch("vertex_y", &_vertex_y, "vertex_y/F");
    _outtree->Branch("vertex_z", &_vertex_z, "vertex_z/F");

    _outtree->Branch("nctor_v"     , &_nctor_v);
    _outtree->Branch("line_frac_vv", &_line_frac_vv);
    _outtree->Branch("line_len_vv" , &_line_len_vv);

    return;
  }

  double SelNueID::Select() {
    LLCV_DEBUG() << "start" << std::endl;

    
    LLCV_DEBUG() << "(RSEV)=("<< Run() << "," << SubRun() << "," << Event() << "," << VertexID() << ")" << std::endl;    
    LLCV_DEBUG() << "VTX=(" 
		 << Data().Vertex()->X() << "," 
		 << Data().Vertex()->Y() << "," 
		 << Data().Vertex()->Z() << ")" << std::endl;

    // int isnue = Tree().Scalar<int>("isnue");
    // LLCV_DEBUG() << "Is this nue?: " <<  isnue << std::endl;
    // if (isnue == 0)  {
    //   LLCV_DEBUG() << "skip" << std::endl;
    //   return 0.0;
    // }
    
    auto mat_v  = Image().Image<cv::Mat>(kImageADC,_cropx,_cropy);
    auto meta_v = Image().Image<larocv::ImageMeta>(kImageADC,_cropx,_cropy);
    auto img_v  = Image().Image<larcv::Image2D>(kImageADC,_cropx,_cropy);
    auto dead_v = Image().Image<cv::Mat>(kImageDead,_cropx,_cropy);

    for(const auto& meta : meta_v)
      _ContourScan.SetPlaneInfo(*meta);
    
    std::array<cv::Mat,3> timg_v;
    std::array<cv::Mat,3> dimg_v;
    std::array<cv::Mat,3> timg3d_v;
    std::array<cv::Mat,3> mat3d_v;

    for(size_t plane=0; plane<3; ++plane) {
      timg_v[plane] = larocv::Threshold(*(mat_v[plane]),10,255);
      mat3d_v[plane] = As8UC3(timg_v[plane]);
      timg3d_v[plane] = larocv::BlankImage(*(mat_v[plane]),0);
      dimg_v[plane] = *(dead_v[plane]);
    }
    
    larocv::data::Vertex3D vtx3d;
    vtx3d.x = Data().Vertex()->X();
    vtx3d.y = Data().Vertex()->Y();
    vtx3d.z = Data().Vertex()->Z();

    _vertex_x = (float)vtx3d.x;
    _vertex_y = (float)vtx3d.y;
    _vertex_z = (float)vtx3d.z;

    //
    // Project the vertex onto the plane
    //
    larocv::GEO2D_Contour_t vertex_pt_v;
    vertex_pt_v.resize(3);

    for(size_t plane=0; plane<3; ++plane) {
      int px_x = kINVALID_INT;
      int px_y = kINVALID_INT;

      ProjectMat(img_v[plane]->meta(),
		 vtx3d.x,vtx3d.y,vtx3d.z,
    		 px_x, px_y);

      vertex_pt_v[plane] = cv::Point_<int>(px_y,px_x);
    }

    //
    // Find contours, get closest to vertex
    //
    
    std::array<larocv::GEO2D_Contour_t,3> vertex_ctor_v;
    
    for(size_t plane=0; plane<3; ++plane) {
      const auto& timg = timg_v[plane];
      auto& vertex_ctor = vertex_ctor_v[plane];
      auto plane_ctor_v = larocv::FindContours(timg);
      
      LLCV_DEBUG() << "@plane=" << plane << " found " << plane_ctor_v.size() << " ctors" << std::endl;
      
      const auto vertex_pt = vertex_pt_v[plane];
      
      auto close_id = FindClosestContour(plane_ctor_v,vertex_pt);

      if (close_id == larocv::kINVALID_SIZE) {
	LLCV_CRITICAL() << "No contour? Skip..." << std::endl;
	continue;
      }

      vertex_ctor = plane_ctor_v[close_id];

      _ContourScan.RegisterContour(timg,vertex_ctor,plane,100);
    } // end plane
    
    //
    // Make a 3D image using vertex contour points
    //
    _ContourScan.Scan(timg_v,dimg_v,timg3d_v);
    
    //
    // Recluster, choose vertex contour
    //
  
    std::array<larocv::GEO2D_ContourArray_t,3> par_ctor_v;

    _nctor_v.resize(3);
    _line_frac_vv.resize(3);
    _line_len_vv.resize(3);

    std::array<larocv::GEO2D_ContourArray_t,3> lc_v;

    for(size_t plane=0; plane<3; ++plane) {
      auto& timg3d = timg3d_v[plane];
      auto& vertex_ctor = vertex_ctor_v[plane];

      auto plane_ctor_v = larocv::FindContours(timg3d);
      
      LLCV_DEBUG() << "@plane=" << plane << " found " << plane_ctor_v.size() << " ctors" << std::endl;
      
      const auto vertex_pt = vertex_pt_v[plane];
      
      auto close_id = FindClosestContour(plane_ctor_v,vertex_pt);

      if (close_id == larocv::kINVALID_SIZE) {
	LLCV_CRITICAL() << "No contour? Skip..." << std::endl;
	continue;
      }

      vertex_ctor = plane_ctor_v[close_id];
      
      // mask all but vertex_ctor
      timg3d = larocv::MaskImage(timg3d,vertex_ctor,-1,false);
      
      // mask out the vertex from image
      auto timg3d_mask = larocv::MaskImage(timg3d,geo2d::Circle<float>(vertex_pt,5),-1,true);
      
      // find contours
      auto& par_ctor = par_ctor_v[plane];
      par_ctor = larocv::FindContours(timg3d_mask);

      auto& nctor = _nctor_v[plane];
      nctor = par_ctor_v[plane].size();
      
      auto& line_frac_v = _line_frac_vv[plane];
      line_frac_v.resize(nctor);

      auto& line_len_v = _line_len_vv[plane];
      line_len_v.resize(nctor);
      
      for(size_t pid=0; pid<(size_t)nctor; ++pid) {
	auto& line_frac = line_frac_v[pid];
	auto& line_len = line_len_v[pid];

	line_frac = -1;
	line_len  = -1;

	auto par = par_ctor[pid];

	// estimate the end point
	geo2d::Vector<float> edge;
	larocv::FindEdge(par,geo2d::Vector<float>(vertex_pt.x,vertex_pt.y),edge);

	geo2d::Vector<int> edge_i;
	edge_i.x = (int)edge.x;
	edge_i.y = (int)edge.y;

	auto timg3d_par   = larocv::MaskImage(timg3d_mask,par,-1,false);
	auto timg3d_blank = larocv::BlankImage(timg3d_mask,0);
	
	// draw a line
	cv::line(timg3d_blank,vertex_pt,edge,cv::Scalar(255),3);
	
	// find the contour
	auto line_ctor_v = larocv::FindContours(timg3d_blank);
	if (line_ctor_v.empty()) continue;
	const auto& line_ctor = line_ctor_v.front();
	
	lc_v[plane].push_back(line_ctor);

	// mask this out of par image
	float npar_pixels = (float)larocv::CountNonZero(timg3d_par);
	timg3d_par = larocv::MaskImage(timg3d_par,line_ctor,-1,false);
	float nline_pixels = (float)larocv::CountNonZero(timg3d_par);
	
	float ratio = 0;
	if (npar_pixels > 0)
	  ratio = nline_pixels / npar_pixels;

	line_frac = ratio;
	line_len = (float)geo2d::dist(vertex_pt,edge_i);

      }
      
    }
    
    
    // for(size_t plane=0; plane<3; ++plane) {
    //   auto& mat3d = mat3d_v[plane];
      
    //   auto nzero = larocv::FindNonZero(timg3d_v[plane]);
    //   for(auto nz : nzero)
    // 	mat3d.at<cv::Vec3b>(nz.y,nz.x) = {255,255,0};

    //   cv::drawContours(mat3d,lc_v[plane],-1,cv::Scalar(0,0,255));
      
    //   std::stringstream ss;
    //   ss.str("");
    //   ss << "cpng/plane_img_" << Run() << "_" << SubRun() << "_" << Event() << "_" << VertexID() << "_" << plane << ".png";
    //   cv::imwrite(ss.str(),mat3d);
    // }
    
    _outtree->Fill();
    

    LLCV_DEBUG() << "end" << std::endl;
    return 0.0;
  }

  size_t SelNueID::FindClosestContour(const larocv::GEO2D_ContourArray_t& ctor_v,
				      const geo2d::Vector<float>& pt) {

    size_t res;
    
    // get the contour closest to the vertex
    float dist = kINVALID_FLOAT;
    size_t close_id = kINVALID_SIZE;
    for(size_t cid=0; cid<ctor_v.size(); ++cid) {
      const auto& ctor = ctor_v[cid];
      auto dist_tmp = larocv::Pt2PtDistance(pt,ctor);
      if (dist_tmp < dist) {
	dist = dist_tmp;
	close_id = cid;
      }
    }
    
    res = close_id;
    return res;
  }
  
  void SelNueID::Finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    _outtree->Write();
    LLCV_DEBUG() << "end" << std::endl;
  }

}


#endif
  
