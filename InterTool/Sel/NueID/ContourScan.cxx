#ifndef __CONTOURSCAN_CXX__
#define __CONTOURSCAN_CXX__

#include "ContourScan.h"

namespace llcv {
  
  void ContourScan::Reset() {

    
    return;
  }
  
  void ContourScan::RegisterContour(const cv::Mat& img, const larocv::GEO2D_Contour_t& ctor, const size_t plane, const float rad) {
    auto mask = larocv::MaskImage(img,ctor,-1,false);

    if (rad > 0)
      mask = larocv::MaskImage(img,geo2d::Circle<float>(200,200,rad),-1,false);

    _ctor_v[plane] = larocv::FindNonZero(mask);
    return;
  }
  
  void ContourScan::Scan(const std::array<cv::Mat,3>& in_img_v,
			 const std::array<cv::Mat,3>& dead_img_v,
			 std::array<cv::Mat,3>& out_img_v) {

    larocv::data::Vertex3D res;

    for(const auto& plane_comb : _plane_comb_v) {
      auto plane0 = plane_comb[0];
      auto plane1 = plane_comb[1];
      auto plane2 = plane_comb[2];

      for(size_t pid0=0; pid0<_ctor_v[plane0].size(); ++pid0) {
	for(size_t pid1=0; pid1<_ctor_v[plane1].size(); ++pid1) {

	  if (!_geo.YZPoint(_ctor_v[plane0][pid0],plane0,
			    _ctor_v[plane1][pid1],plane1,
			    res)) continue;
	  

	  int px = (int)res.vtx2d_v[plane2].pt.x;
	  int py = (int)res.vtx2d_v[plane2].pt.y;

	  int img_pt   = (int)in_img_v[plane2].at<uchar>(py,px);
	  
	  int not_dead = (int)dead_img_v[plane2].at<uchar>(py,px);
							 
	  if (img_pt<10 and not_dead) continue;
	  
	  out_img_v[plane0].at<uchar>(_ctor_v[plane0][pid0].y,
				      _ctor_v[plane0][pid0].x)  = (uchar) 255;
	  
	  out_img_v[plane1].at<uchar>(_ctor_v[plane1][pid1].y,
				      _ctor_v[plane1][pid1].x)  = (uchar) 255;

	  	  
	} // end plane0
      } // end plane1
    } // end plane combo

    
    return;
  }



}

#endif
