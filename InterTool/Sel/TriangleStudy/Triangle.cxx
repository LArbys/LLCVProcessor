#ifndef __TRIANGLE_CXX__
#define __TRIANGLE_CXX__

#include "Triangle.h"

#include <cassert>

#ifndef __CLING__
#ifndef __CINT__
#include <opencv2/opencv.hpp>
#endif
#endif
#include <opencv2/core/core.hpp>

namespace llcv { 
  
  Triangle::Triangle(const larocv::GEO2D_Contour_t& ctor,
		     const geo2d::Vector<float>& start) {
    
    auto min_rect  = larocv::MinAreaRect(ctor);
    geo2d::Vector<float> pt_v[4];
    min_rect.points(pt_v);

    // get the two closest points to the vertex
    float dist = larocv::kINVALID_FLOAT;
    
    size_t id1 = larocv::kINVALID_SIZE;
    size_t id2 = larocv::kINVALID_SIZE;

    for(size_t pid=0; pid<4; ++pid) {
      auto dist_tmp = geo2d::dist(start,pt_v[pid]);
      if (dist_tmp < dist) {
	dist = dist_tmp;
	id1 = pid;
      }
    }

    dist = larocv::kINVALID_FLOAT;

    for(size_t pid=0; pid<4; ++pid) {
      if (pid == id1) continue;
      auto dist_tmp = geo2d::dist(start,pt_v[pid]);
      if (dist_tmp < dist) {
	dist = dist_tmp;
	id2 = pid;
      }
    }
    
    geo2d::Vector<float> far_pt1(larocv::kINVALID_FLOAT,
				 larocv::kINVALID_FLOAT);
    geo2d::Vector<float> far_pt2(larocv::kINVALID_FLOAT,
				 larocv::kINVALID_FLOAT);

    for(size_t pid=0; pid<4; ++pid) {
      if (pid == id1) continue;
      if (pid == id2) continue;
      if (far_pt1.x == larocv::kINVALID_FLOAT) {
	far_pt1 = pt_v[pid];
	continue;
      }
      if (far_pt2.x == larocv::kINVALID_FLOAT) {
	far_pt2 = pt_v[pid];
	continue;
      }
    }
    
    _base_pt1 = far_pt1;
    _base_pt2 = far_pt2;
    _apex = start;
    _ctor = ctor;
  }

  void Triangle::Expand(const cv::Mat& img, const float fraction) { 

    assert(fraction>0);
    assert(fraction<1);

    //
    // get the mid point between base1 and base2
    //
    
    auto bmax_x = std::max(_base_pt1.x,_base_pt2.x);
    auto bmin_x = std::min(_base_pt1.x,_base_pt2.x);

    auto bmax_y = std::max(_base_pt1.y,_base_pt2.y);
    auto bmin_y = std::min(_base_pt1.y,_base_pt2.y);

    auto bdx = bmax_x - bmin_x;
    auto bdy = bmax_y - bmin_y;
    
    bdx /= 2.0;
    bdy /= 2.0;
    
    geo2d::Vector<float> mid_pt(bmin_x + bdx, bmin_y + bdy);
    
    // 
    // find the point fraction * length
    //
    auto apex_to_mid1 = mid_pt - _apex;
    apex_to_mid1 *= fraction;
    apex_to_mid1 += _apex;

    //
    // make the ctor_img
    //
    auto ctor_img = larocv::BlankImage(img,255);
    ctor_img = larocv::MaskImage(ctor_img,_ctor,-1,false);

    //
    // Chop off the pixels along the apex line
    //
    auto dir = _base_pt1 - _base_pt2;
    geo2d::Vector<float> apex_to_mid2;
      
    if (dir.x != 0) {
      geo2d::Line<float> line(apex_to_mid1,dir);
      
      apex_to_mid1.x = img.rows;
      apex_to_mid1.y = line.y(apex_to_mid1.x);

      apex_to_mid2.x = 0;
      apex_to_mid2.y = line.y(apex_to_mid2.x);
    }
    // it's a vertical line
    else {
      apex_to_mid1.x = apex_to_mid1.x;
      apex_to_mid1.y = img.rows;

      apex_to_mid2.x = apex_to_mid1.x;
      apex_to_mid2.y = 0;
    }
    
    cv::line(ctor_img,apex_to_mid1,apex_to_mid2,cv::Scalar(0),2);
    
    //
    // Find contours, choose one closest to mid_pt
    //
    auto ctor_v = larocv::FindContours(ctor_img);
    size_t close_id = larocv::kINVALID_SIZE;
    float dist = larocv::kINVALID_FLOAT;
    for(size_t cid=0; cid < ctor_v.size(); ++cid) {
      auto dist_tmp = larocv::Pt2PtDistance(mid_pt,ctor_v[cid]);
      if (dist_tmp < dist) {
	dist = dist_tmp;
	close_id = cid;
      }
    }

    assert(close_id != larocv::kINVALID_SIZE);
    
    const auto& far_ctor = ctor_v[close_id];

    ctor_img = larocv::MaskImage(ctor_img,far_ctor,-1,false);

    MovePt(ctor_img,_base_pt1);
    MovePt(ctor_img,_base_pt2);

    return;
  }
  
  
  void Triangle::MovePt(const cv::Mat& ctor_img, geo2d::Vector<float>& base_pt) {

    geo2d::Vector<float> dir(0,0);
    bool right = false;

    // input is base 1
    if (base_pt == _base_pt1) {
      dir = base_pt - _base_pt2;
      if (base_pt.x > _base_pt2.x)
	right = true;
    }
    
    // input is base 2
    else { 
      dir = base_pt - _base_pt1;    
      if (base_pt.x > _base_pt1.x)
	right = true;
    }

    bool isinf = false;
    geo2d::Line<float> line(base_pt,dir);

    if (dir.x == 0)
      isinf = true;

    while(larocv::Broken(ctor_img,_apex,base_pt,1)) {

      if (isinf) {
	base_pt.y =+ 1;
	continue;
      }

      if (right)
	base_pt.x += 1;
      else
	base_pt.x -= 1;

      base_pt.y  = line.y(base_pt.x);

      if (base_pt.x >= ctor_img.rows) break;
      if (base_pt.y >= ctor_img.cols) break;

      if(base_pt.x < 0) break;
      if(base_pt.y < 0) break;
    }
    
    
    return;
  }

}


#endif
