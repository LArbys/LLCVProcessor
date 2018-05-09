#ifndef __SELNUEID_CXX__
#define __SELNUEID_CXX__

#include "SelNueID.h"

#include "InterTool_Util/InterImageUtils.h"

#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

#include <iomanip>
#include <sstream>
#include <cstdlib>

#include "Geo2D/Core/LineSegment.h"

#include "LArUtil/GeometryHelper.h"
#include "Base/DataFormatConstants.h"

#include "TVector2.h"

namespace llcv {

  void SelNueID::Configure (const larcv::PSet &pset) {
    set_verbosity((msg::Level_t)pset.get<int>("Verbosity",2));
    LLCV_DEBUG() << "start" << std::endl;
    _cropx = 400;
    _cropy = 400;

    _extension_cutoff = pset.get<float>("ExtensionFraction",0.8);
    _Match.Configure(pset.get<larcv::PSet>("MatchIOU"));

    _twatch.Stop();
  
    LLCV_DEBUG() << "end" << std::endl;
  }

  void SelNueID::Initialize() {
    _outtree = new TTree("SelNueID","");
    AttachRSEV(_outtree);
    
    _outtree->Branch("vertex_x", &_vertex_x, "vertex_x/F");
    _outtree->Branch("vertex_y", &_vertex_y, "vertex_y/F");
    _outtree->Branch("vertex_z", &_vertex_z, "vertex_z/F");

    _outtree->Branch("nctor_v"       , &_nctor_v);
    _outtree->Branch("line_frac_vv"  , &_line_frac_vv);
    _outtree->Branch("line_len_vv"   , &_line_len_vv);
    _outtree->Branch("line_startx_vv", &_line_startx_vv);
    _outtree->Branch("line_starty_vv", &_line_starty_vv);
    _outtree->Branch("line_endx_vv"  , &_line_endx_vv);
    _outtree->Branch("line_endy_vv"  , &_line_endy_vv);

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

    _white_img = larocv::BlankImage(*(mat_v.front()),0);
    _white_img.setTo(cv::Scalar(0));

    _ContourScan.Reset();

    for(const auto& meta : meta_v)
      _ContourScan.SetPlaneInfo(*meta);
    
    std::array<cv::Mat,3> aimg_v;
    std::array<cv::Mat,3> timg_v;
    std::array<cv::Mat,3> dimg_v;
    std::array<cv::Mat,3> timg3d_v;
    std::array<cv::Mat,3> mat3d_v;

    for(size_t plane=0; plane<3; ++plane) {
      aimg_v[plane]    = *(mat_v[plane]);
      timg_v[plane]   = larocv::Threshold(*(mat_v[plane]),10,255);
      mat3d_v[plane]  = As8UC3(timg_v[plane]);
      timg3d_v[plane] = _white_img.clone();
      dimg_v[plane]   = *(dead_v[plane]);
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
      const auto vertex_pt = vertex_pt_v[plane];
      
      auto close_id = FindClosestContour(plane_ctor_v,vertex_pt);

      if (close_id == larocv::kINVALID_SIZE) {
	LLCV_CRITICAL() << "No contour? Skip..." << std::endl;
	continue;
      }

      vertex_ctor = plane_ctor_v[close_id];
      _ContourScan.RegisterContour(timg,vertex_ctor,plane,200);
    }    
    
    
    //
    // Make a 3D image using vertex contour points
    //
    _ContourScan.Scan(timg_v,dimg_v,timg3d_v);
    
    //
    // Recluster, choose vertex contour
    //

    std::array<larocv::GEO2D_ContourArray_t,3> img_ctor_v;
    std::array<size_t,3> close_id_v;
    std::array<larocv::GEO2D_ContourArray_t,3> par_ctor_v;
    std::array<larocv::GEO2D_ContourArray_t,3> line_contour_vv;
    std::array<std::vector<Triangle>,3> triangle_vv;
    std::array<geo2d::VectorArray<float>,3> edge_vv;


    _nctor_v.clear();
    _nctor_v.resize(3);
    
    for(size_t plane=0; plane<3; ++plane) {
      auto& timg3d = timg3d_v[plane];
      auto& plane_ctor_v = img_ctor_v[plane];
      auto& vertex_ctor  = vertex_ctor_v[plane];
      auto& close_id = close_id_v[plane];

      plane_ctor_v = larocv::FindContours(timg3d);
      
      LLCV_DEBUG() << "@plane=" << plane 
		   << " found " << plane_ctor_v.size() 
		   << " ctors" << std::endl;
      
      const auto vertex_pt = vertex_pt_v[plane];
      
      close_id = FindClosestContour(plane_ctor_v,vertex_pt);

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
      
      auto& line_contour_v = line_contour_vv[plane];
      auto& triangle_v     = triangle_vv[plane];
      auto& edge_v         = edge_vv[plane];
      line_contour_v.reserve(nctor);
      triangle_v.reserve(nctor);
      edge_v.reserve(nctor);

      for(size_t pid=0; pid<(size_t)nctor; ++pid) {

	LLCV_DEBUG() << "@pid=" << pid << std::endl;
	
	auto par = par_ctor[pid];

	// estimate the end point
	Triangle triangle(par,vertex_pt);

	geo2d::Vector<float> edge;
	float nline_pixels = 0;
	auto line_ctor = MaximizeLine(timg3d,triangle,nline_pixels,edge);
	
	if (line_ctor.empty()) continue;

	line_contour_v.emplace_back(std::move(line_ctor));
	triangle_v.emplace_back(std::move(triangle));

	edge_v.emplace_back(std::move(edge));
      } // end contour
    } // end plane
    

    //
    // get the max, min, time for contours
    //
    float tlo = larocv::kINVALID_FLOAT;
    float thi = -1.0*larocv::kINVALID_FLOAT;
    for(size_t plane=0; plane<3; ++plane) {
      for (const auto& edge : edge_vv[plane]) {
	tlo = std::min(tlo,edge.x);
	thi = std::max(thi,edge.x);
      }
    }

    // per plane, per line, collection of contours
    std::array<std::vector<std::vector<const larocv::GEO2D_Contour_t*> > ,3> ctor_overlap_vvv;

    //
    // extend line to touch potential charge
    // if line is left, go -x
    // if line is right, go +x
    //

    for(size_t plane=0; plane<3; ++plane) {
      const auto& line_contour_v = line_contour_vv[plane];
      const auto& triangle_v = triangle_vv[plane];
      const auto& edge_v = edge_vv[plane];
      auto& ctor_overlap_vv = ctor_overlap_vvv[plane];

      const auto& plane_ctor_v = img_ctor_v[plane];
      const auto& close_id = close_id_v[plane];
      
      ctor_overlap_vv.resize(line_contour_v.size());

      for(size_t cid=0; cid<line_contour_v.size(); ++cid) {
	auto& ctor_overlap_v = ctor_overlap_vv[cid];
	
	_white_img.setTo(cv::Scalar(0));

	// determine the direction
	const auto& edge = edge_v[cid];
	const auto& triangle = triangle_v[cid];
	
	geo2d::Vector<float> new_edge;
	geo2d::Line<float> line(triangle.Apex(),edge - triangle.Apex());
	
	// extend backwards
	if (edge.x < triangle.Apex().x) {
	  new_edge.x = tlo;
	  new_edge.y = line.y(new_edge.x);
	}

	// extend forwards
	else {
	  new_edge.x = thi;
	  new_edge.y = line.y(new_edge.x);
	}
	
	// Determine if line intersects new charge
	cv::line(_white_img,triangle.Apex(),new_edge,cv::Scalar(255),3);
	auto lc_v = larocv::FindContours(_white_img);
	if (lc_v.empty()) continue;

	const auto& lc = lc_v.front();

	for(size_t ict=0; ict<plane_ctor_v.size(); ++ict) {
	  if (ict == close_id) continue;
	  if (!larocv::AreaOverlap(plane_ctor_v[ict],lc)) continue;
	  ctor_overlap_v.push_back(&(plane_ctor_v[ict]));
	} // end intersection
      } // end this line
    } // end this plane

    //
    // Append new pixels
    //
    for(size_t plane=0; plane<3; ++plane) {
      const auto& timg = timg_v[plane];
      const auto& line_contour_v = line_contour_vv[plane];
      for(size_t cid=0; cid<line_contour_v.size(); ++cid) {
	for(const auto ctor : ctor_overlap_vvv[plane][cid]) {
	  auto nz_pts = larocv::FindNonZero(larocv::MaskImage(timg,*ctor,-1,false));
	  _ContourScan.AddPixels(nz_pts,plane);
	}
      }
    }

    //
    // Re-scan
    //
    _ContourScan.Scan(timg_v,dimg_v,timg3d_v);
    
    //
    // For each line determine what fraction the new contours are 3D -- if they
    // are above threshold -- make a new line which connects them
    // and fill out Object2D
    //

    std::array<std::vector<Object2D>,3> object_vv;

    for(size_t plane=0; plane<3; ++plane) {
      const auto& timg   = timg_v[plane];
      const auto& timg3d = timg3d_v[plane];

      auto& line_contour_v = line_contour_vv[plane];
      auto& triangle_v     = triangle_vv[plane];
      auto& edge_v         = edge_vv[plane];
      auto& object_v       = object_vv[plane];

      object_v.resize(line_contour_v.size());

      for(size_t cid=0; cid<line_contour_v.size(); ++cid) {

	auto& triangle = triangle_v[cid];
	auto& edge = edge_v[cid];

	auto new_ctor = triangle.Contour();
	auto& object = object_v[cid];

	object._polygon_v.emplace_back(new_ctor,triangle.Apex());

	float npar_pixels = (float)larocv::CountNonZero(larocv::MaskImage(timg,new_ctor,0,false));

	for(const auto ctor : ctor_overlap_vvv[plane][cid]) {
	  float total_plane = larocv::CountNonZero(larocv::MaskImage(timg,*ctor,0,false));
	  float total_3d    = larocv::CountNonZero(larocv::MaskImage(timg3d,*ctor,0,false));
	  float ratio = 0.0;
	  if (total_plane > 0)
	    ratio = (total_3d / total_plane);
	  
	  if (ratio < _extension_cutoff) continue;
	  for(const auto& pt : *ctor)
	    new_ctor.emplace_back(pt);
	  
	  npar_pixels += (float)larocv::CountNonZero(larocv::MaskImage(timg,*ctor,0,false));
	  object._polygon_v.emplace_back(*ctor,triangle.Apex());
	}

	triangle = Triangle(new_ctor,triangle.Apex());

	float nline_pixels = 0.0;
	auto line_ctor = MaximizeLine(timg3d,triangle,nline_pixels,edge);

	float nratio = 0;
	if (npar_pixels > 0)
	  nratio = nline_pixels / npar_pixels;

	object._triangle  = triangle;
	object._line      = line_ctor;
	object._line_frac = nratio;
	object._edge      = edge;
	object._plane     = plane;
     
      }
    }

    _Match.ClearEvent();
    _Match.ClearMatch();

    LLCV_DEBUG() << "Match lines" << std::endl;
    for(size_t plane=0; plane<3; ++plane) {
      LLCV_DEBUG() << "@plane=" << plane << std::endl;
      for(const auto& object : object_vv[plane]) {
	_Match.Register(object,plane);
      }
      LLCV_DEBUG() << "num register=" << object_vv[plane].size() << std::endl;
    }
    
    std::vector<float> score_v;
    auto match_vv = _Match.MatchObjects(score_v);

    LLCV_DEBUG() << "& recieved " << match_vv.size() << " matched particles" << std::endl;
    
    std::vector<Object2DCollection> obj_col_v;
    obj_col_v.resize(match_vv.size());

    for(size_t mid=0; mid< match_vv.size(); ++mid) {
      auto match_v = match_vv[mid];
      auto score = score_v[mid];

      auto& obj_col = obj_col_v[mid];
      
      obj_col.SetStart(Data().Vertex()->X(),
		       Data().Vertex()->Y(),
		       Data().Vertex()->Z());
      
      obj_col.SetScore(score);

      // Fill the match
      for (auto match : match_v) {
	auto plane = match.first;
	auto id    = match.second;
	LLCV_DEBUG() << "@plane=" << plane << " id=" << id << std::endl;
	obj_col.emplace_back(object_vv[plane][id]);
      }
      ReconstructAngle(aimg_v,obj_col);
      ReconstructLength(img_v,aimg_v,obj_col);
      LLCV_DEBUG() << "theta=" << obj_col.Theta() << " phi=" << obj_col.Phi() << std::endl;
      LLCV_DEBUG() << "(" << obj_col.dX() << "," << obj_col.dY() << "," << obj_col.dZ() << ")" << std::endl;
      LLCV_DEBUG() << "length=" << obj_col.Length() << std::endl;
      LLCV_DEBUG() << "score=" << score << std::endl;
      LLCV_DEBUG() << "..." << std::endl;
    }
    
    
    // _line_frac_vv.clear();
    // _line_len_vv.clear();
    // _line_startx_vv.clear();;
    // _line_starty_vv.clear();
    // _line_endx_vv.clear();
    // _line_endy_vv.clear();

    // _line_frac_vv.resize(3);
    // _line_len_vv.resize(3);
    // _line_startx_vv.resize(3);
    // _line_starty_vv.resize(3);
    // _line_endx_vv.resize(3);
    // _line_endy_vv.resize(3);
    
    
    //
    // Debug print out
    //
    for(size_t plane=0; plane<3; ++plane) {
      auto& mat3d = mat3d_v[plane];
      
      auto nzero = larocv::FindNonZero(timg3d_v[plane]);
      for(auto nz : nzero)
    	mat3d.at<cv::Vec3b>(nz.y,nz.x) = {255,255,0};
      
      cv::drawContours(mat3d,line_contour_vv[plane],-1,cv::Scalar(0,0,255));
      
      std::stringstream ss;
      ss.str("");
      ss << "cpng/plane_img_" << Run() << "_" << SubRun() << "_" << Event() << "_" << VertexID() << "_" << plane << ".png";
      cv::imwrite(ss.str(),mat3d);
    }
    
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
  
  larocv::GEO2D_Contour_t SelNueID::MaximizeLine(const cv::Mat& timg3d_mask,
						 const Triangle& triangle,
						 float& nline_pixels,
						 geo2d::Vector<float>& edge) {
    
    larocv::GEO2D_Contour_t res;

    auto timg3d_par   = larocv::MaskImage(timg3d_mask,triangle.Contour(),-1,false);
    
    nline_pixels = -1*larocv::kINVALID_FLOAT;

    const geo2d::Vector<float>* base_left  = nullptr;
    const geo2d::Vector<float>* base_right = nullptr;

    const geo2d::Vector<float>* base_up   = nullptr;
    const geo2d::Vector<float>* base_down = nullptr;

    bool isinf = false;
    
    if (triangle.Base1().x < triangle.Base2().x) {
      base_left  = &triangle.Base1();
      base_right = &triangle.Base2();
    }
    else if(triangle.Base1().x > triangle.Base2().x) {
      base_left  = &triangle.Base2();
      base_right = &triangle.Base1();
    }
    // it's infinity
    else {
      LLCV_WARNING() << "isinf detected" << std::endl;
      isinf = true;
      if (triangle.Base1().y < triangle.Base2().y) {
	base_down = &triangle.Base1();
	base_up   = &triangle.Base2();
      }
      else {
	base_down = &triangle.Base2();
	base_up   = &triangle.Base1();
      }
    }

    geo2d::LineSegment<float> base_seg;
    
    float lo;
    float hi; 

    if (!isinf)  {
      base_seg = geo2d::LineSegment<float>(*base_left,*base_right);
      lo = base_left->x;
      hi = base_right->x;
    }
    else {
      base_seg = geo2d::LineSegment<float>(*base_down,*base_up);
      lo = base_down->y;
      hi = base_up->y;
    }

    geo2d::Vector<float> pt;
    
    float step = 0.1;
    for(float rid=lo; rid<hi; rid+=step) {
      
      _white_img.setTo(cv::Scalar(0));
      
      if (!isinf) {
	pt.x = rid;
	pt.y = -1;
	try {
	  pt.y = base_seg.y(pt.x);
	}
	catch (...) {
	  continue;
	}
      } 
      
      else {
	pt.x = base_down->x;
	pt.y = rid;
      }

      // draw a line
      cv::line(_white_img,triangle.Apex(),pt,cv::Scalar(255),3);
      
      // find the contour
      auto line_ctor_v = larocv::FindContours(_white_img);
      if (line_ctor_v.empty()) continue;
      const auto& line_ctor = line_ctor_v.front();
      
      if (line_ctor.empty()) continue;
      
      auto timg3d_par_line = larocv::MaskImage(timg3d_par,line_ctor,-1,false);
      float nline_pixels_tmp = (float)larocv::CountNonZero(timg3d_par_line);
      
      if (nline_pixels_tmp > nline_pixels) {
	nline_pixels = nline_pixels_tmp;
	edge = pt;
	res = line_ctor;
      }
      
    }

    return res;
  }
  

  //
  // adapted from https://goo.gl/Q7MBpE
  //
  void SelNueID::ReconstructAngle(const std::array<cv::Mat,3>& img_v,
				  Object2DCollection& obj_col) {

    auto geomH = larutil::GeometryHelper::GetME();
    
    // planes with largest number of hits used to get 3D direction
    std::vector<int> planeHits(3,0);
    std::vector<larutil::Point2D> planeDir(3);
    
    for(const auto& obj : obj_col) {

      const auto pl = obj._plane;
      
      larutil::Point2D weightedDir;
      weightedDir.w = 0;
      weightedDir.t = 0;
      float Qtot = 0;
      
      int nhits = 0;
      for(const auto& poly : obj._polygon_v) {
	auto nz_pt_v = larocv::FindNonZero(larocv::MaskImage(img_v[pl],poly.Contour(),-1,false));
	
	for (const auto& nz_pt  : nz_pt_v){
	  float charge = (float) (img_v[pl].at<uchar>(nz_pt.y,nz_pt.x));
	  weightedDir.w += (nz_pt.y - obj.Start().y) * charge;
	  weightedDir.t += (nz_pt.x - obj.Start().x) * charge;
	  Qtot += charge;
	  nhits++;
	}
      }
      
      weightedDir.w /= Qtot;
      weightedDir.t /= Qtot;

      planeHits[pl] = nhits;
      planeDir[pl]  = weightedDir;
    }

    int pl_max = larlite::data::kINVALID_INT;
    int pl_mid = larlite::data::kINVALID_INT;
    int pl_min = larlite::data::kINVALID_INT;

    int n_max  = -1.0*larlite::data::kINVALID_INT;
    int n_min  =      larlite::data::kINVALID_INT;

    for (size_t pl=0; pl < planeHits.size(); pl++){
      if (planeHits[pl] > n_max){
	pl_max = pl;
	n_max  = planeHits[pl];
      }
      if (planeHits[pl] < n_min){
	pl_min = pl;
	n_min  = planeHits[pl];
      }
    }

    assert(pl_max != larlite::data::kINVALID_INT);
    assert(pl_min != larlite::data::kINVALID_INT);

    // find the medium plane
    for(int pp=0; pp<3; ++pp) {
      if (pp == pl_max) continue;
      if (pp == pl_min) continue;
      pl_mid = pp;
    }

    assert(pl_mid != larlite::data::kINVALID_INT);

    float slope_max, slope_mid;
    float angle_max, angle_mid;
    slope_max = planeDir[pl_max].t / planeDir[pl_max].w;
    angle_max = std::atan(slope_max);
    angle_max = std::atan2( planeDir[pl_max].t , planeDir[pl_max].w );
    slope_mid = planeDir[pl_mid].t / planeDir[pl_mid].w;
    angle_mid = std::atan(slope_mid);
    angle_mid = std::atan2( planeDir[pl_mid].t , planeDir[pl_mid].w );
    
    double theta, phi;
    geomH->Get3DAxisN(pl_max, pl_mid,
		      angle_max, angle_mid,
		      phi, theta);
    
    obj_col.SetTheta(theta);
    obj_col.SetPhi(phi);
    
  }

  // adapted from https://goo.gl/1pCDEa

  void SelNueID::ReconstructLength(const std::vector<larcv::Image2D*>& img_v,
				   const std::array<cv::Mat,3>& aimg_v,
				   Object2DCollection& obj_col) {
    
    auto geomH = larutil::GeometryHelper::GetME();

    // output
    std::array<double,3> length_v;

    // geometry
    std::array<double,3> plane_f_v;
    std::array<TVector2,3> line_dir_v;

    //initialize
    for(auto& v : length_v) 
      v = -1.0*::larlite::data::kINVALID_DOUBLE;
    
    plane_f_v = length_v;
    
    // calcluate line
    float alpha = 5;

    TVector3 dir3D(obj_col.dX(),obj_col.dY(),obj_col.dZ());
    
    for(size_t plane=0; plane<3; ++plane) {
      plane_f_v[plane] = geomH->Project_3DLine_OnPlane(dir3D, plane).Mag();
      
      const auto& startPoint2D = obj_col.front().Start();
      TVector3 secondPoint3D = obj_col.Start() + alpha * dir3D;
      
      int px_x, px_y;
      ProjectMat(img_v[plane]->meta(),
		 secondPoint3D.X(),secondPoint3D.Y(),secondPoint3D.Z(),
		 px_x, px_y);
      
      geo2d::Vector<float> secondPoint2D(px_y,px_x);
      std::cout << "@plane=" << plane << " start=" << startPoint2D << " end=" << secondPoint2D << std::endl;

      TVector2 dir(secondPoint2D.y - startPoint2D.y, secondPoint2D.x - startPoint2D.x);
      dir *= 1.0 / dir.Mod();
      line_dir_v[plane] = dir;
    }

    // calculate the length
    for(const auto& obj : obj_col) {

      const auto pl = obj._plane;

      const auto& dr_w = line_dir_v[pl].X();
      const auto& dr_t = line_dir_v[pl].Y();
      
      std::vector<std::pair<float,float> > dist_v;

      float qsum = 0;
      float d2D = 0;
      
      for(const auto& poly : obj._polygon_v) {
	auto nz_pt_v = larocv::FindNonZero(larocv::MaskImage(aimg_v[pl],poly.Contour(),-1,false));
	for (const auto& nz_pt  : nz_pt_v)
	  qsum += (float) (aimg_v[pl].at<uchar>(nz_pt.y,nz_pt.x));
      }

      for(const auto& poly : obj._polygon_v) {
	auto nz_pt_v = larocv::FindNonZero(larocv::MaskImage(aimg_v[pl],poly.Contour(),-1,false));
     
	for(size_t nid=0; nid < nz_pt_v.size(); ++nid) {
	  auto nz_pt = nz_pt_v[nid];

	  float charge = (float) (aimg_v[pl].at<uchar>(nz_pt.y,nz_pt.x));

	  float ptw = (nz_pt.y - obj.Start().y)*0.3;
	  float ptt = (nz_pt.x - obj.Start().x)*0.3;
	
	  // calculate distance along the line
	  d2D  = (ptw * dr_w) + (ptt * dr_t);
	  d2D  = std::abs(d2D);
	  dist_v.emplace_back(std::make_pair(d2D, charge / qsum));
	}
      } // end "hit" loop
      
      std::sort(std::begin(dist_v),std::end(dist_v),
		[](const std::pair<float,float>& lhs, const std::pair<float,float>& rhs)
		{ return lhs.first < rhs.first; });
      
      qsum = 0;
      d2D = 0;
      float _qfraction = 1.0;
      for(const auto& dist_pair : dist_v) {
	d2D   = dist_pair.first;
	qsum += dist_pair.second;
	if (qsum>_qfraction) break;
      }
      
      auto f = plane_f_v.at(pl);
      
      double length = d2D / f;

      std::cout << "@pl=" << pl << " f=" << f << " length=" << length << std::endl;
      length_v[pl] = length;
    }
    
    // auto length = *(std::max_element(std::begin(length_v),std::end(length_v)));

    float sum = 0.0;
    for(auto length : length_v)  {
      if (length == -1.0*::larlite::data::kINVALID_DOUBLE)
	continue;

      sum += (float)length;
    }
    
    sum /= ((float)length_v.size());

    obj_col.SetLength(sum);
  }

  void SelNueID::Finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    _outtree->Write();
    LLCV_DEBUG() << "end" << std::endl;
  }

}


#endif
  
