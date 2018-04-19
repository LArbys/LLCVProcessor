#ifndef __SELTRACKSCATTER_CXX__
#define __SELTRACKSCATTER_CXX__

#include "SelTrackScatter.h"

#include "InterTool_Util/InterImageUtils.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

#include <sstream>
#include <cstdlib>
#include <array>

namespace llcv {

  void SelTrackScatter::Configure (const larcv::PSet &pset) {
    set_verbosity((msg::Level_t)pset.get<int>("Verbosity",2));
    LLCV_DEBUG() << "start" << std::endl;
    
    _cropx = 400;
    _cropy = 400;

    auto lrdb_pset = pset.get<larcv::PSet>("LongRangeDB");
    auto srdb_pset = pset.get<larcv::PSet>("ShortRangeDB");

    _LR_DefectBreaker.Configure(lrdb_pset.get<int>("MinDefectSize"),
				lrdb_pset.get<int>("NHullEdgePoints"),
				lrdb_pset.get<int>("NAllowedBreaks"));

    _SR_DefectBreaker.Configure(srdb_pset.get<int>("MinDefectSize"),
				srdb_pset.get<int>("NHullEdgePoints"),
				srdb_pset.get<int>("NAllowedBreaks"));
    
    _DBSCAN.Configure(10,5);

    LLCV_DEBUG() << "end" << std::endl;
  }

  void SelTrackScatter::Initialize() {
    LLCV_DEBUG() << "start" << std::endl;

    _outtree = new TTree("scatter","");
    AttachRSEV(_outtree);

    _outtree->Branch("track_x_vv", &_track_x_vv);
    _outtree->Branch("track_y_vv", &_track_y_vv);
    _outtree->Branch("track_z_vv", &_track_z_vv);

    _outtree->Branch("shower_x_vv", &_shower_x_vv);
    _outtree->Branch("shower_y_vv", &_shower_y_vv);
    _outtree->Branch("shower_z_vv", &_shower_z_vv);

    _outtree->Branch("shower_start_x_vv" , &_shower_start_x_vv);
    _outtree->Branch("shower_start_y_vv" , &_shower_start_y_vv);
    _outtree->Branch("shower_start_z_vv" , &_shower_start_z_vv);

    _outtree->Branch("shower_center_x_vv", &_shower_center_x_vv);
    _outtree->Branch("shower_center_y_vv", &_shower_center_y_vv);
    _outtree->Branch("shower_center_z_vv", &_shower_center_z_vv);

    _outtree->Branch("shower_end_x_vv"   , &_shower_end_x_vv);
    _outtree->Branch("shower_end_y_vv"   , &_shower_end_y_vv);
    _outtree->Branch("shower_end_z_vv"   , &_shower_end_z_vv);

    _outtree->Branch("shower_edge1_x_vv"   , &_shower_edge1_x_vv);
    _outtree->Branch("shower_edge1_y_vv"   , &_shower_edge1_y_vv);
    _outtree->Branch("shower_edge1_z_vv"   , &_shower_edge1_z_vv);

    _outtree->Branch("shower_edge2_x_vv"   , &_shower_edge2_x_vv);
    _outtree->Branch("shower_edge2_y_vv"   , &_shower_edge2_y_vv);
    _outtree->Branch("shower_edge2_z_vv"   , &_shower_edge2_z_vv);

    _outtree->Branch("shower_dev_vv", &_shower_dev_vv);
    _outtree->Branch("shower_cid_vv", &_shower_cid_vv);

    _outtree->Branch("shower_length_v", &_shower_length_v);
    _outtree->Branch("shower_width_v", &_shower_width_v);
    _outtree->Branch("shower_width1_v", &_shower_width1_v);
    _outtree->Branch("shower_width2_v", &_shower_width2_v);

    _outtree->Branch("shower_theta_v", &_shower_theta_v);
    _outtree->Branch("shower_phi_v", &_shower_phi_v);

    _outtree->Branch("shower_opening_v", &_shower_opening_v);
    _outtree->Branch("shower_opening1_v", &_shower_opening1_v);
    _outtree->Branch("shower_opening2_v", &_shower_opening2_v);

    
    LLCV_DEBUG() << "end" << std::endl;
  }

  double SelTrackScatter::Select() {
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
    
    //std::array<cv::Mat,3> white_mat_v;
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
      // white_mat_v[plane] = larocv::BlankImage(*mat);
    }
    

    //
    // Project the vertex onto the plane
    //
    GEO2D_Contour_t vertex_ctor;
    vertex_ctor.resize(3);

    for(size_t plane=0; plane<3; ++plane) {
      int px_x = kINVALID_INT;
      int px_y = kINVALID_INT;

      ProjectMat(img_v.at(plane)->meta(),
    		 Data().Vertex()->X(),Data().Vertex()->Y(),Data().Vertex()->Z(),
    		 px_x, px_y);

      vertex_ctor[plane] = cv::Point_<int>(px_y,px_x);
    }
    

    //
    // Project reconstructed tracks onto plane
    //
    
    auto track_v = Data().Tracks();
    
    std::vector<std::vector<GEO2D_Contour_t> > track_ctor_vv;
    track_ctor_vv.resize(track_v.size());

    for(size_t tid=0; tid<track_v.size(); ++tid) {
      const auto track = track_v[tid];
      auto& track_ctor_v = track_ctor_vv[tid];
      track_ctor_v.resize(3);

      for(auto& v : track_ctor_v) 
    	v.reserve(track->NumberTrajectoryPoints());

      for(size_t pid=0; pid< track->NumberTrajectoryPoints(); ++pid) {
	
    	const auto& pt = track->LocationAtPoint(pid);
	
    	for(size_t plane=0; plane<3; ++plane) {
	  
    	  auto& track_ctor = track_ctor_v[plane];
	  
    	  int px_x = kINVALID_INT;
    	  int px_y = kINVALID_INT;
	  
    	  ProjectMat(img_v.at(plane)->meta(),
    		     pt.X(),pt.Y(),pt.Z(),
    		     px_x, px_y);
	  
    	  if (px_x < 0) continue;
    	  if (px_y < 0) continue;

    	  if (px_x >= (int)_cropx) continue;
    	  if (px_y >= (int)_cropy) continue;
	  
    	  auto& mat3d = mat3d_v[plane];
	  
    	  if (tid==0) mat3d.at<cv::Vec3b>(px_x,px_y) = {0,0,255};
    	  if (tid==1) mat3d.at<cv::Vec3b>(px_x,px_y) = {0,255,0};
    	  if (tid==2) mat3d.at<cv::Vec3b>(px_x,px_y) = {255,0,0};
	  
    	  track_ctor.emplace_back(px_y,px_x);
	  
    	} // end plane
      } // end track point
    } // end track
    

    //
    // generate clusters, associate to tracks
    //
    std::vector<larocv::GEO2D_ContourArray_t> ctor_vv;
    ctor_vv.resize(3);

    std::vector<std::vector<std::vector<size_t> > > tass_vvv;
    tass_vvv.resize(3);

    for(size_t plane=0; plane<3; ++plane) {
      const auto& mat = thresh_mat_v[plane];
      auto& ctor_v = ctor_vv[plane];
      auto& tass_vv= tass_vvv[plane];

      ctor_v  = FindAndMaskVertex(mat,vertex_ctor[plane]);
      tass_vv = AssociateToTracks(ctor_v, track_ctor_vv, plane);

      auto& img3d = mat3d_v[plane];
      for(size_t cid=0; cid<ctor_v.size(); ++cid) {
      	cv::Scalar color(std::rand() % 255,std::rand() % 255,std::rand() % 255);
      	if (!ContainsTrack(tass_vv[cid])) continue;
      	// cv::drawContours(img3d,ctor_v,cid,color);
      }

    }


    //
    // generate 3D consistent points using radial approximation
    //
    
    larocv::data::Vertex3D vtx3d;
    vtx3d.x = Data().Vertex()->X();
    vtx3d.y = Data().Vertex()->Y();
    vtx3d.z = Data().Vertex()->Z();
  
    float _track_threshold = 0.5;

    ResizeOutput(track_v.size());

    for(size_t tid=0; tid<track_v.size(); ++tid) {

      auto& track_x_v = _track_x_vv[tid];
      auto& track_y_v = _track_y_vv[tid];
      auto& track_z_v = _track_z_vv[tid];

      for(size_t pid=0; pid< track_v[tid]->NumberTrajectoryPoints(); ++pid) {
	const auto& pt = track_v[tid]->LocationAtPoint(pid);
	track_x_v.push_back(pt.X());
	track_y_v.push_back(pt.Y());
	track_z_v.push_back(pt.Z());
      }

      LLCV_DEBUG() << "@tid=" << tid << std::endl;
      
      std::array<cv::Mat,3> timg_v;

      for(size_t plane=0; plane<3; ++plane) {
	
    	larocv::GEO2D_ContourArray_t veto_v;

    	const auto& ctor_v = ctor_vv[plane];
    	const auto& tass_vv= tass_vvv[plane];
	
    	for(size_t cid=0; cid<ctor_v.size(); ++cid) {
    	  const auto& tass_v = tass_vv[cid];

    	  if (TrackFraction(tass_v,tid) < _track_threshold) 
    	    continue;

    	  veto_v.emplace_back(ctor_v[cid]);
    	}
	
    	const auto& thresh_mat = thresh_mat_v[plane];
    	timg_v[plane] = larocv::MaskImage(thresh_mat,veto_v,-1,false);
      }
      
      // estimate the track angle
      auto theta_phi = TrackAngle(*track_v[tid]);
      auto track_len = TrackLength(*track_v[tid]);

      float pi8 = 3.14159 / 8.0;
      float pi4 = 3.14159 / 4.0;
      
      float rad_min = 0.5;
      float rad_max = track_len + 3;
      float rad_step = 0.5;
      
      float theta_min = theta_phi.first - pi8;
      float theta_max = theta_phi.first + pi8;

      float phi_min = theta_phi.second - pi4;
      float phi_max = theta_phi.second + pi4;

      LLCV_DEBUG() << "Scanning Spheres" << std::endl;

      _PixelScan3D.Reconfigure(rad_min, rad_max, rad_step,
			       theta_min, theta_max,
			       phi_min, phi_max);
      
      //auto reg_v = _PixelScan3D.SphereScan3D(white_mat_v,vtx3d,3);
      auto reg_v = _PixelScan3D.SphereScan3D(timg_v,dead_v,vtx3d,3);
      
      LLCV_DEBUG() << "Returned: " << reg_v.size() << " locations" << std::endl;


      if (!reg_v.empty()) {
	LLCV_DEBUG() << "Objectify" << std::endl;
	Object3D obj(vtx3d,reg_v);
	LLCV_DEBUG() << "return" << std::endl;

	auto& shower_x_v = _shower_x_vv[tid];
	auto& shower_y_v = _shower_y_vv[tid];
	auto& shower_z_v = _shower_z_vv[tid];

	auto& shower_start_x_v = _shower_start_x_vv[tid];
	auto& shower_start_y_v = _shower_start_y_vv[tid];
	auto& shower_start_z_v = _shower_start_z_vv[tid];

	auto& shower_center_x_v = _shower_center_x_vv[tid];
	auto& shower_center_y_v = _shower_center_y_vv[tid];
	auto& shower_center_z_v = _shower_center_z_vv[tid];

	auto& shower_end_x_v = _shower_end_x_vv[tid];
	auto& shower_end_y_v = _shower_end_y_vv[tid];
	auto& shower_end_z_v = _shower_end_z_vv[tid];

	auto& shower_edge1_x_v = _shower_edge1_x_vv[tid];
	auto& shower_edge1_y_v = _shower_edge1_y_vv[tid];
	auto& shower_edge1_z_v = _shower_edge1_z_vv[tid];

	auto& shower_edge2_x_v = _shower_edge2_x_vv[tid];
	auto& shower_edge2_y_v = _shower_edge2_y_vv[tid];
	auto& shower_edge2_z_v = _shower_edge2_z_vv[tid];

	auto& shower_dev_v = _shower_dev_vv[tid];
	auto& shower_cid_v = _shower_cid_vv[tid];

	shower_dev_v = obj.PCADeviation();
	shower_cid_v = Cluster(obj);

	shower_start_x_v.push_back(obj.Start().x);
	shower_start_y_v.push_back(obj.Start().y);
	shower_start_z_v.push_back(obj.Start().z);

	shower_center_x_v.push_back(obj.Center().x);
	shower_center_y_v.push_back(obj.Center().y);
	shower_center_z_v.push_back(obj.Center().z);

	shower_end_x_v.push_back(obj.End().x);
	shower_end_y_v.push_back(obj.End().y);
	shower_end_z_v.push_back(obj.End().z);

	shower_edge1_x_v.push_back(obj.Edge1().x);
	shower_edge1_y_v.push_back(obj.Edge1().y);
	shower_edge1_z_v.push_back(obj.Edge1().z);

	shower_edge2_x_v.push_back(obj.Edge2().x);
	shower_edge2_y_v.push_back(obj.Edge2().y);
	shower_edge2_z_v.push_back(obj.Edge2().z);

	_shower_length_v[tid] = obj.Length();
	_shower_width_v[tid]  = obj.Width();
	_shower_width1_v[tid] = obj.Width1();
	_shower_width2_v[tid] = obj.Width2();
      
	_shower_theta_v[tid] = obj.Theta();
	_shower_phi_v[tid]   = obj.Phi();
      
	_shower_opening_v[tid]  = obj.Opening();
	_shower_opening1_v[tid] = obj.Opening1();
	_shower_opening2_v[tid] = obj.Opening2();

	for(const auto& reg : reg_v) {
	
	  shower_x_v.push_back(reg.x);
	  shower_y_v.push_back(reg.y);
	  shower_z_v.push_back(reg.z);

	  for(size_t plane=0; plane<3; ++plane) {
	    auto& mat3d = mat3d_v[plane];
	    int px_x = reg.vtx2d_v[plane].pt.x;
	    int px_y = reg.vtx2d_v[plane].pt.y;
	    if (px_x < 0) continue;
	    mat3d.at<cv::Vec3b>(px_y,px_x) = {255,255,0};
	  }
	}
      }

      LLCV_DEBUG() << "done!" << std::endl;

    }
    

    //    
    // debug print out
    //
    for(size_t plane=0; plane<3; ++plane) {
      auto& img3d = mat3d_v[plane];
      std::stringstream ss; 
      ss << "png/plane_img_" << Run() << "_" << SubRun() << "_" << Event() << "_" << VertexID() << "_" << plane << ".png";
      cv::imwrite(ss.str(),img3d);
    }

    _outtree->Fill();
    
    LLCV_DEBUG() << "end" << std::endl;
    return 0.0;
  }


  larocv::GEO2D_ContourArray_t SelTrackScatter::FindAndMaskVertex(const cv::Mat& mat,
								  const cv::Point_<int> vertex) {
    larocv::GEO2D_ContourArray_t ctor_v;

    auto vtx_circ = geo2d::Circle<float>(vertex.x, vertex.y, 4);
    auto mat_mask = larocv::MaskImage(mat,vtx_circ);

    auto ctor_tmp_v = larocv::FindContours(mat_mask);

    // Split off the small contours
    larocv::GEO2D_ContourArray_t small_ctor_v, large_ctor_v;
    small_ctor_v.reserve((int) ((float)ctor_tmp_v.size() / 2.0));
    large_ctor_v.reserve((int) ((float)ctor_tmp_v.size() / 2.0));
    
    for(auto& ctor_tmp : ctor_tmp_v) {
      if (ctor_tmp.size()<=2) 
	small_ctor_v.emplace_back(std::move(ctor_tmp));
      else 
	large_ctor_v.emplace_back(std::move(ctor_tmp));
    }
    
    std::swap(ctor_tmp_v,large_ctor_v);

    larocv::GEO2D_ContourArray_t ctor_tmp_split_v;
    ctor_tmp_split_v.reserve(ctor_tmp_v.size());

    for(size_t cid=0; cid< ctor_tmp_v.size(); ++cid) {
      auto split_v = _LR_DefectBreaker.SplitContour(ctor_tmp_v[cid]);

      for(auto& split : split_v)
	ctor_tmp_split_v.emplace_back(std::move(split));
    }
    
    size_t sz_estimate = ctor_tmp_split_v.size() + small_ctor_v.size();
    ctor_v.reserve(sz_estimate);

    for(auto& ctor_tmp_split : ctor_tmp_split_v)
      ctor_v.emplace_back(std::move(ctor_tmp_split));

    for(auto& small_ctor : small_ctor_v)
      ctor_v.emplace_back(std::move(small_ctor));    
    
    return ctor_v;
  }


  larocv::GEO2D_ContourArray_t SelTrackScatter::FindAndBreakVertex(const cv::Mat& mat,
								   const cv::Point_<int> vertex) {

    larocv::GEO2D_ContourArray_t ctor_v;
    
    auto ctor_tmp_v = larocv::FindContours(mat);

    // Split off the small contours
    larocv::GEO2D_ContourArray_t small_ctor_v, large_ctor_v;
    small_ctor_v.reserve((int) ((float)ctor_tmp_v.size() / 2.0));
    large_ctor_v.reserve((int) ((float)ctor_tmp_v.size() / 2.0));
    
    for(auto& ctor_tmp : ctor_tmp_v) {
      if (ctor_tmp.size()<=2) 
	small_ctor_v.emplace_back(std::move(ctor_tmp));
      else 
	large_ctor_v.emplace_back(std::move(ctor_tmp));
    }
      
    std::swap(ctor_tmp_v,large_ctor_v);

    // Find the contour which contains the vertex
    auto vid = larocv::FindContainingContour(ctor_tmp_v,vertex);
    if (vid==kINVALID_SIZE) 
      throw llcv_err("Could not find containing contour");

    // Break this contour -- long range
    auto blr_ctor_tmp_v = _LR_DefectBreaker.SplitContour(ctor_tmp_v[vid]);

    // Find the broken contour with vertex
    auto blr_vid = larocv::FindContainingContour(blr_ctor_tmp_v,vertex);
    if (blr_vid==kINVALID_SIZE) 
      throw llcv_err("Could not find containing contour");

    // Break this contour -- short range
    auto bsr_ctor_tmp_v = _SR_DefectBreaker.SplitContour(blr_ctor_tmp_v[blr_vid]);

    size_t sz_estimate = ctor_tmp_v.size() + blr_ctor_tmp_v.size() + bsr_ctor_tmp_v.size() + small_ctor_v.size();
    ctor_v.reserve(sz_estimate);

    for(size_t cid = 0; cid < ctor_tmp_v.size(); ++cid) {
      if (cid == vid) continue;
      ctor_v.emplace_back(std::move(ctor_tmp_v[cid]));
    }

    for(size_t cid = 0; cid < blr_ctor_tmp_v.size(); ++cid) {
      if (cid == blr_vid) continue;
      ctor_v.emplace_back(std::move(blr_ctor_tmp_v[cid]));
    }
      
    for(auto& bsr_ctor_tmp : bsr_ctor_tmp_v)
      ctor_v.emplace_back(std::move(bsr_ctor_tmp));

    for(auto& small_ctor : small_ctor_v)
      ctor_v.emplace_back(std::move(small_ctor));

    return ctor_v;

  }
  

  std::vector<std::vector<size_t> > SelTrackScatter::AssociateToTracks(const larocv::GEO2D_ContourArray_t& ctor_v,
								       const std::vector<larocv::GEO2D_ContourArray_t>& track_ctor_vv,
								       const size_t plane) {
    LLCV_DEBUG() << "start" << std::endl;
    // Save the contours where projected pixel is inside
    std::vector<std::vector<size_t> > tin_vv;
    tin_vv.resize(ctor_v.size());
    for(auto& v : tin_vv) 
      v.resize(track_ctor_vv.size(),0);
    
    for(size_t cid = 0; cid < ctor_v.size(); ++cid) {
      const auto& ctor = ctor_v[cid];
      auto& tin_v = tin_vv[cid];

      for(size_t tid = 0; tid < track_ctor_vv.size(); ++tid) {
	const auto& track_ctor = track_ctor_vv[tid][plane];

	for(const auto& pt : track_ctor) {
	  double dist;
	  auto inside = larocv::PointPolygonTest(ctor,pt,dist);	    
	  if (inside or dist<=1) {
	    tin_v[tid] += 1;
	  }
	}
      } // end track
    } // end contour
    
    LLCV_DEBUG() << "end" << std::endl;
    return tin_vv;
  }

  std::pair<float,float> SelTrackScatter::TrackAngle(const larlite::track& track) {
    
    std::pair<float,float> res;
    
    const auto& start_pt= track.Vertex();
    const auto& end_pt  = track.End();
    auto dir = end_pt - start_pt;

    res.first  = std::acos( dir.Z() / dir.Mag());
    res.second = std::atan2(dir.X(),dir.Y()); // wtf
    
    return res;
  }

  float SelTrackScatter::TrackLength(const larlite::track& track) {
    
    float res;
    
    const auto& start_pt= track.Vertex();
    const auto& end_pt  = track.End();
    auto dir = end_pt - start_pt;

    res = dir.Mag();

    return res;
  }

  bool SelTrackScatter::ContainsTrack(const std::vector<size_t>& tin_v) {
    
    for(const auto tin : tin_v) {
      if (tin > 0) return true;
    }

    return false;
  }

  float SelTrackScatter::TrackFraction(const std::vector<size_t>& tin_v, size_t tid) {
    
    float ret = 0;
    float sum = 0;
    for(const auto tin : tin_v)
      sum += (float) tin;
    
    if (sum==0) return ret;
    
    ret = ((float)tin_v[tid]) / sum;
    
    return ret;
  }
  
  void SelTrackScatter::Finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    _outtree->Write();
    LLCV_DEBUG() << "end" << std::endl;
  }

  std::vector<int> SelTrackScatter::Cluster(const Object3D& obj) {
    std::vector<int> res_v;
    res_v.resize(obj.Points().size(),-1);
    
    static std::vector<Point> pts_v;
    pts_v.clear();
    pts_v.resize(obj.Points().size());
    
    for(size_t pid=0; pid<obj.Points().size(); ++pid) {
      pts_v[pid].x = obj.Points()[pid].x;
      pts_v[pid].y = obj.Points()[pid].y;
      pts_v[pid].z = obj.Points()[pid].z;
    }
    
    
    _DBSCAN.Reset(pts_v);
    _DBSCAN.run();

    for(size_t pid=0; pid<obj.Points().size(); ++pid) 
      res_v[pid] = _DBSCAN.Points()[pid].clusterID - 1;

    return res_v;
  }

  void SelTrackScatter::ResizeOutput(size_t sz) {

    _shower_x_vv.clear();
    _shower_y_vv.clear();
    _shower_z_vv.clear();
    
    _track_x_vv.clear();
    _track_y_vv.clear();
    _track_z_vv.clear();

    _shower_start_x_vv.clear();
    _shower_end_x_vv.clear();
    _shower_center_x_vv.clear();

    _shower_start_y_vv.clear();
    _shower_end_y_vv.clear();
    _shower_center_y_vv.clear();

    _shower_start_z_vv.clear();
    _shower_end_z_vv.clear();
    _shower_center_z_vv.clear();

    _shower_edge1_x_vv.clear();
    _shower_edge1_y_vv.clear();
    _shower_edge1_z_vv.clear();

    _shower_edge2_x_vv.clear();
    _shower_edge2_y_vv.clear();
    _shower_edge2_z_vv.clear();

    _shower_length_v.clear();
    _shower_width_v.clear();
    _shower_width1_v.clear();
    _shower_width2_v.clear();

    _shower_theta_v.clear();
    _shower_phi_v.clear();

    _shower_opening_v.clear();
    _shower_opening1_v.clear();
    _shower_opening2_v.clear();

    _shower_dev_vv.clear();

    _shower_cid_vv.clear();

    _track_x_vv.resize(sz);
    _track_y_vv.resize(sz);
    _track_z_vv.resize(sz);

    _shower_x_vv.resize(sz);
    _shower_y_vv.resize(sz);
    _shower_z_vv.resize(sz);

    _shower_start_x_vv.resize(sz);
    _shower_end_x_vv.resize(sz);
    _shower_center_x_vv.resize(sz);

    _shower_start_y_vv.resize(sz);
    _shower_end_y_vv.resize(sz);
    _shower_center_y_vv.resize(sz);

    _shower_start_z_vv.resize(sz);
    _shower_end_z_vv.resize(sz);
    _shower_center_z_vv.resize(sz);

    _shower_edge1_x_vv.resize(sz);
    _shower_edge1_y_vv.resize(sz);
    _shower_edge1_z_vv.resize(sz);

    _shower_edge2_x_vv.resize(sz);
    _shower_edge2_y_vv.resize(sz);
    _shower_edge2_z_vv.resize(sz);

    _shower_length_v.resize(sz);
    _shower_width_v.resize(sz);
    _shower_width1_v.resize(sz);
    _shower_width2_v.resize(sz);

    _shower_theta_v.resize(sz);
    _shower_phi_v.resize(sz);

    _shower_opening_v.resize(sz);
    _shower_opening1_v.resize(sz);
    _shower_opening2_v.resize(sz);

    _shower_dev_vv.resize(sz);

    _shower_cid_vv.resize(sz);

  }


}


#endif

