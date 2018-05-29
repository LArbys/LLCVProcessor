#ifndef __LINEFOLLOW_CXX__
#define __LINEFOLLOW_CXX__

#include "LineFollow.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace llcv {
  
  LineFollow::LineFollow() : llcv_base("LineFollow") {
    this->set_verbosity((msg::Level_t)0);
    _thickness = 2;
    return;
  }
  
  void LineFollow::SetImageDimension(const cv::Mat& img) {
    _img       = img.clone();
    _black_img = larocv::BlankImage(img,0);
    _white_img = larocv::BlankImage(img,255);
  }

  larocv::GEO2D_ContourArray_t LineFollow::FollowEdgeLine(const geo2d::Vector<float>& start) {
    LLCV_DEBUG() << "start" << std::endl;
    larocv::GEO2D_ContourArray_t ret_v;

    float min_radius = 10;
    float max_radius = 10;

    //
    // find the first point off the boundary
    //

    geo2d::Vector<float> init_pt;
    auto first_exists = InitializeFirstPoint(start,init_pt);

    ret_v.emplace_back(AsLine(start,init_pt));
    
    //
    // Move along the line until 
    //

    LLCV_DEBUG() << "end" << std::endl;
    return ret_v;
  }

  bool LineFollow::InitializeFirstPoint(geo2d::Vector<float> start, geo2d::Vector<float>& init_pt) {
    bool exists = false;

    // define the small image
    float pad_x = 10;
    float pad_y = 10;
    
    float min_x = start.x;
    float min_y = start.y;
    
    min_x -= pad_x;
    min_y -= pad_y;
    
    float dx = 2*pad_x;
    float dy = 2*pad_y;

    LLCV_DEBUG() << "start=(" << start.x << "," << start.y << ") : m=(" << min_x << "," << min_y << ") : d=(" << dx << "," << dy << ")" << std::endl;
    auto small_img = larocv::SmallImg(_img,geo2d::Vector<float>(min_x,min_y),dx,dy);
    cv::imwrite("test0a.png",small_img);

    if (start.x>0)
      start.x -= (min_x-dx);
    
    if (start.y>0)
      start.y -= (min_y-dy);

    // define the circle
    LLCV_DEBUG() << "small start=(" << start.x << "," << start.y << ")" << std::endl;
    auto small_bimg = larocv::BlankImage(small_img,0);
    cv::circle(small_bimg,cv::Point((int)(start.x+0.5),(int)(start.y+0.5)),(int)pad_x*3,cv::Scalar(255),1);
    cv::imwrite("test0b.png",small_bimg);

    auto nz_pt_v = larocv::FindNonZero(small_bimg);
    
    return exists;
  }

  larocv::GEO2D_Contour_t LineFollow::AsLine(geo2d::Vector<float> pt1, geo2d::Vector<float> pt2) {
    larocv::GEO2D_Contour_t ret;
    
    float pad_x = 20;
    float pad_y = 20;

    float min_x = std::min(pt1.x,pt2.x);
    float min_y = std::min(pt1.y,pt2.y);

    float dx = std::abs(pt1.x - pt2.x)/2.0;
    float dy = std::abs(pt1.y - pt2.y)/2.0;

    min_x -= pad_x;
    min_y -= pad_y;
    
    dx += pad_x;
    dy += pad_y;

    auto small_img = larocv::SmallImg(_img,geo2d::Vector<float>(min_x+dx,min_y+dy),dx,dy);

    pt1.x -= min_x;
    pt1.y -= (min_y+pad_y);
    
    pt2.x -= min_x;
    pt2.y -= (min_y+pad_y);
    
    cv::line(small_img,
    	     cv::Point((int)(pt1.x+0.5),(int)(pt1.y+0.5)),
	     cv::Point((int)(pt2.x+0.5),(int)(pt2.y+0.5)),
	     //cv::Scalar(255),
	     cv::Scalar(150),
	     _thickness);

    cv::imwrite("test1.png",small_img);
    

    return ret;
  }
  
}

#endif


