#ifndef __POLYGON_CXX__
#define __POLYGON_CXX__

#include "Polygon.h"

#ifndef __CLING__
#ifndef __CINT__
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#endif
#endif

#include <cassert>


namespace llcv {
  
  Polygon::Polygon(const larocv::GEO2D_Contour_t& ctor,const geo2d::Vector<float>& start) {

    _ctor = ctor;
    _start = start;

    Construct();

  }      
      
  void Polygon::Construct() {

    _hull.clear();
    _hull_v.clear();
    _defect_info_v.clear();
    _defect_dist_v.clear();

    cv::convexHull(_ctor, _hull_v);
    _hull_v.push_back(_hull_v.front());

    cv::convexityDefects(_ctor,_hull_v,_defect_info_v);

    _defect_dist_v.resize(_defect_info_v.size(),-1);
    
    for(size_t did=0; did<_defect_info_v.size(); ++did)
      _defect_dist_v[did]  = ((float)_defect_info_v[did][3])/256.0;

    _hull.reserve(_hull_v.size());
    for(auto hid : _hull_v)
      _hull.emplace_back(_ctor.at(hid));
    
    VetoStartPoint();
  }

  void Polygon::VetoStartPoint() {
    _veto_ctor_id = larocv::kINVALID_INT;
    _veto_hull_id = larocv::kINVALID_INT;

    _veto_ctor_id = larocv::Pt2PtDistance(_start,_ctor);
    _veto_hull_id = larocv::Pt2PtDistance(_start,_hull);

    return;
  }

  int Polygon::NumberDefects(float dist_thresh) const {
    int res = 0;
    
    for(auto defect_dist : _defect_dist_v) {
      if (defect_dist > dist_thresh) {
	res += 1;
      }
    }

    return res;
  }


  int Polygon::NumberDefectsNoStart(float dist_thresh) const {
    int res = 0;
    
    for(size_t did=0; did<_defect_dist_v.size(); ++did) {
      
      auto start_idx = _defect_info_v[did][0];
      auto end_idx   = _defect_info_v[did][1];

      if (_veto_ctor_id >= start_idx and _veto_ctor_id <= end_idx) continue;
      
      auto defect_dist = _defect_dist_v[did];
      if (defect_dist > dist_thresh)
	res += 1;

    }

    return res;
  }


  float Polygon::LargestDefect() const {
    float res = 0;

    if (_defect_dist_v.empty()) return res;

    auto max_iter = std::max_element(_defect_dist_v.begin(),_defect_dist_v.end());
    
    res = *max_iter;
    
    return res;
  }

  float Polygon::LargestDefectNoStart() const {
    float res = 0;

    float largest = -1*larocv::kINVALID_FLOAT;
    
    if (_defect_dist_v.empty()) return res;

    for(size_t did=0; did<_defect_dist_v.size(); ++did) {
      
      auto start_idx = _defect_info_v[did][0];
      auto end_idx   = _defect_info_v[did][1];

      if (_veto_ctor_id >= start_idx and _veto_ctor_id <= end_idx) continue;

      auto defect_dist = _defect_dist_v[did];
      largest = std::max(largest, defect_dist);
    }

    res = largest;
    
    return res;
  }


  float Polygon::SmallestDefect() const {
    float res = 0;
    
    if (_defect_dist_v.empty()) return res;

    auto min_iter = std::min_element(_defect_dist_v.begin(),_defect_dist_v.end());

    res = *min_iter;

    return res;
  }

  float Polygon::SmallestDefectNoStart() const {
    float res = 0;

    float smallest = larocv::kINVALID_FLOAT;

    if (_defect_dist_v.empty()) return res;

    for(size_t did=0; did<_defect_dist_v.size(); ++did) {
      
      auto start_idx = _defect_info_v[did][0];
      auto end_idx   = _defect_info_v[did][1];

      if (_veto_ctor_id >= start_idx and _veto_ctor_id <= end_idx) continue;
      
      auto defect_dist = _defect_dist_v[did];
      smallest = std::min(smallest,defect_dist);
    }

    res = smallest;
    
    return res;
  }

  float Polygon::EmptyAreaRatio() const {
    float res = -1;
    
    float hull_area = larocv::ContourArea(_hull);
    float ctor_area = larocv::ContourArea(_ctor);
    
    if (ctor_area > 0)
      res = hull_area / ctor_area;
      
    
    return res;
  }


  float Polygon::EmptyArea() const {
    float res = -1;
    
    float hull_area = larocv::ContourArea(_hull);
    float ctor_area = larocv::ContourArea(_ctor);
    
    res = hull_area - ctor_area;
    
    return res;
  }

  float Polygon::PocketArea() const {
    float res = 0;

    for(const auto& defect_info : _defect_info_v) {
      larocv::GEO2D_Contour_t pocket;
      //pocket.reserve(defect_info[1] - defect_info[0]);

      for(int did=defect_info[0]; did<defect_info[1]; ++did)
	pocket.emplace_back(_ctor.at(did));
      
      if (!pocket.empty()) {
	pocket.emplace_back(pocket.front());
	res += larocv::ContourArea(pocket);
      }

    }

    return res;
  }

  float Polygon::PocketAreaNoStart() const {
    float res = 0;

    for(size_t dinfo=0; dinfo<_defect_info_v.size(); ++dinfo) {

      auto start_idx = _defect_info_v[dinfo][0];
      auto end_idx   = _defect_info_v[dinfo][1];

      if (_veto_ctor_id >= start_idx and _veto_ctor_id <= end_idx) continue;

      larocv::GEO2D_Contour_t pocket;
      //pocket.reserve(end_idx - start_idx);

      for(int did=start_idx; did<end_idx; ++did)
	pocket.emplace_back(_ctor.at(did));
      
      if (!pocket.empty()) {
	pocket.emplace_back(pocket.front());
	res += larocv::ContourArea(pocket);
      }
    }

    return res;
  }

}

#endif
