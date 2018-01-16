#ifndef __SEARCHALGOSINGLE_CXX__
#define __SEARCHALGOSINGLE_CXX__

#include "SearchAlgoSingle.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"

namespace llcv {
  
  void SearchAlgoSingle::Configure(const larcv::PSet &pset) {
    LLCV_DEBUG() << "start" << std::endl;
    _shower_frac   = pset.get<float>("ShowerFrac",0.8);
    _shower_size   = pset.get<float>("ShowerSize",40);
    _shower_impact = pset.get<float>("ShowerImpact",4); // 4cm
    
    _PixelScan3D.set_verbosity((larocv::msg::Level_t)this->logger().level());
    _PixelScan3D.Configure(pset.get<larocv::Config_t>("PixelScan3D"));
    LLCV_DEBUG() << "end" << std::endl;
    return;
  }

  std::vector<llcv::DetachedCandidate>
    SearchAlgoSingle::_Search_(const larocv::data::Vertex3D& vtx3d,
			       std::vector<cv::Mat>& adc_mat_v,
			       std::vector<cv::Mat>& shr_mat_v,
			       const std::vector<larocv::ImageMeta>& meta_v) { 
    
    LLCV_DEBUG() << "start" << std::endl;
    
    //
    // setup the return & prepare images
    // 
    std::vector<llcv::DetachedCandidate> res_v;

    for(auto& img : adc_mat_v)
      img = larocv::Threshold(img,10,255);    

    for(auto& img : shr_mat_v)
      img = larocv::Threshold(img,10,255);    

    for(auto const& meta : meta_v)
      _PixelScan3D.SetPlaneInfo(meta);

    //
    // find adc contours
    //
    std::vector<larocv::GEO2D_ContourArray_t> ctor_vv;
    ctor_vv.resize(3);

    for(size_t plane=0; plane<3; ++plane) {
      auto ctor_v  = larocv::FindContours(adc_mat_v.at(plane)); 
      ctor_vv[plane] = std::move(ctor_v);
    }
    
    std::array<size_t, 3> vtx_ctor_v;
    std::array<bool,3> valid_plane_v;
    std::array<cv::Mat,3> simg_v;
    std::array<larocv::GEO2D_ContourArray_t,3> sctor_vv;
    std::array<larocv::GEO2D_ContourArray_t,3> actor_vv;

    LLCV_DEBUG() << "@vtx3d=(" << vtx3d.x << "," << vtx3d.y << "," << vtx3d.z << ")" << std::endl;
    
    cv::imwrite("/tmp/adc0_img0.png",adc_mat_v[0]);
    cv::imwrite("/tmp/adc0_img1.png",adc_mat_v[1]);
    cv::imwrite("/tmp/adc0_img2.png",adc_mat_v[2]);

    cv::imwrite("/tmp/shr0_img0.png",shr_mat_v[0]);
    cv::imwrite("/tmp/shr0_img1.png",shr_mat_v[1]);
    cv::imwrite("/tmp/shr0_img2.png",shr_mat_v[2]);

    // project the vertex into the contour, if inside, remove from image
    for(size_t plane=0; plane<3; ++plane) {
      LLCV_DEBUG() << "@plane=" << plane << std::endl;
      
      simg_v[plane] = shr_mat_v[plane].clone();

      const auto& ctor_v = ctor_vv.at(plane);
      const auto& vtx2d = vtx3d.vtx2d_v.at(plane);
      
      LLCV_DEBUG() << "2d pt=" << vtx2d.pt << std::endl;
      
      if (vtx2d.pt.x < 0 or vtx2d.pt.x == kINVALID_FLOAT) continue;
      if (vtx2d.pt.y < 0 or vtx2d.pt.y == kINVALID_FLOAT) continue;
      
      double distance = kINVALID_DOUBLE;
      auto id = larocv::FindContainingContour(ctor_v, vtx2d.pt,distance);

      if (id == kINVALID_SIZE) {
	LLCV_DEBUG() << "Could not be found..." << std::endl;
	continue;      
      }

      if (distance < -10) {
	LLCV_DEBUG() << "Too far away..." << std::endl;
	continue;
      }

      valid_plane_v[plane] = true;

      LLCV_DEBUG() << "masking vertex contour @id=" << id << std::endl;

      const auto& ctor = ctor_v.at(id);
      vtx_ctor_v[plane] = id;
      simg_v[plane] = larocv::MaskImage(simg_v[plane],ctor,0,true);
    }

    // find shower contours
    for(size_t plane=0; plane<3; ++plane) {
      auto sctor_v  = larocv::FindContours(simg_v[plane]); 
      sctor_vv[plane] = std::move(sctor_v);
    }
    
    // clear possible detached shower contours
    for(auto& v : actor_vv) v.clear();
    
    for(size_t plane = 0; plane<3; ++plane) {
      
      if(!valid_plane_v[plane]) continue;

      const auto& sctor_v  = sctor_vv[plane];
      const auto& ctor_v   = ctor_vv[plane];

      auto& actor_v = actor_vv[plane];
      actor_v.reserve(ctor_v.size());
      
      // for each adc contour inspect showerness
      for(size_t aid = 0 ; aid < ctor_v.size(); ++aid) {
	
	// its the vertex contour
	if (aid == vtx_ctor_v[plane]) continue; 

	const auto& ctor = ctor_v[aid];
	
	// it's too small to be a second shower
	if(larocv::ContourArea(ctor) < _shower_size) {
	  simg_v[plane] = larocv::MaskImage(simg_v[plane],ctor,0,true);
	  continue; 
	}

	auto sid = larocv::FindContainingContour(sctor_v,ctor);
	if (sid == kINVALID_SIZE) continue;

	// it's not shower enough
	double frac = larocv::AreaRatio(sctor_v.at(sid),ctor);
	if (frac < _shower_frac)  {
	  simg_v[plane] = larocv::MaskImage(simg_v[plane],ctor,0,true); 
	  continue;
	}

	// candidate shower cluster
	actor_v.emplace_back(ctor);
      }
    }

    //
    // not enough contours, continue
    //
    size_t cnt=0;
    for(const auto& actor_v : actor_vv) 
      { if (!actor_v.empty()) cnt +=1; }
    
    if (cnt<2) return res_v;
    
    //
    // Register regions for the scan (spheres)
    //
    auto reg_vv = _PixelScan3D.RegionScan3D(simg_v,vtx3d);
      
    //
    // associate to contours to valid 3D points on sphere
    //
    auto ass_vv = _PixelScan3D.AssociateContours(reg_vv,actor_vv);
      
    //
    // count the number of consistent 3D points per contour ID
    //
    std::vector<std::array<size_t,3>> trip_v;
    std::vector<std::vector<const larocv::data::Vertex3D*> > trip_vtx_ptr_vv;
    std::vector<size_t> trip_cnt_v;

    bool found = false;
    for(size_t assvid =0; assvid < ass_vv.size(); ++assvid) {
      const auto& ass_v = ass_vv[assvid];
      for(size_t assid =0; assid < ass_v.size(); ++assid) {
	const auto& ass = ass_v[assid];
	const auto& reg = reg_vv.at(assvid).at(assid);

	found = false;
	for(size_t tid=0; tid<trip_v.size(); ++tid) {
	  if(!CompareAsses(ass,trip_v[tid])) continue;
	  found = true;
	  trip_cnt_v[tid] += 1;
	  trip_vtx_ptr_vv[tid].emplace_back(&reg);
	  break;
	}

	if (found) continue;
	trip_v.emplace_back(ass);
	trip_cnt_v.emplace_back(1);
	trip_vtx_ptr_vv.emplace_back(std::vector<const larocv::data::Vertex3D*>(1,&reg));
      }
    }

    LLCV_DEBUG() << "trip_v sz=    " << trip_v.size() << std::endl;
    LLCV_DEBUG() << "trip_cnt_v sz=" << trip_cnt_v.size() << std::endl;
    for(size_t tid=0; tid< trip_v.size(); ++tid) {
      LLCV_DEBUG() << "@tid=" << tid 
		     << " {" << trip_v[tid][0] << "," << trip_v[tid][1]  << "," << trip_v[tid][2] << "} = " 
		     << trip_cnt_v[tid] << std::endl;
    }
      
    
    //
    // no candidate could be associated across planes
    //
    if (trip_v.empty()) return res_v;

    //
    // pick the most 3D consistent contours
    //
    size_t maxid = std::distance(trip_cnt_v.begin(), std::max_element(trip_cnt_v.begin(), trip_cnt_v.end()));
    LLCV_DEBUG() << "Selected max element @pos=" << maxid << std::endl;
    const auto& trip           = trip_v.at(maxid);
    const auto& trip_vtx_ptr_v = trip_vtx_ptr_vv.at(maxid);
      
    std::vector<larocv::data::SpacePt> sps_v;
    sps_v.resize(trip_vtx_ptr_v.size());
    for(size_t sid=0; sid<sps_v.size(); ++sid) {
      auto& sps = sps_v[sid];
      const auto vtx = trip_vtx_ptr_v[sid];
      sps.pt = *vtx;
    }

    //
    // get the radial point closest to the vertex in 3D
    //
    double min_dist = kINVALID_DOUBLE;
    const larocv::data::SpacePt* min_sp = nullptr;
    for(const auto& sp : sps_v) {
      auto d = larocv::data::Distance(sp,vtx3d);
      if (d<min_dist) {
	min_dist = d;
	min_sp = &sp;
      }
    }
    
    LLCV_DEBUG() << "Second shower candidate idenfitied" << std::endl;

    // make a single shower
    res_v.resize(1);
    auto& res = res_v.front();
    
    res.origin = vtx3d;
    res.start  = min_sp->pt;

    for(int plane=0; plane<3; ++plane) {
      DetachedCluster dc;
      auto aid = trip.at(plane);
      dc.ctor = actor_vv.at(plane).at(aid);
      dc.start_x = kINVALID_FLOAT;
      dc.start_y = kINVALID_FLOAT;
      dc.plane = plane;
      res.Move(std::move(dc),plane);
    }
    
    LLCV_DEBUG() << "end" << std::endl;
    return res_v;
  }

}
#endif
