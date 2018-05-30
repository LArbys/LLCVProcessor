#ifndef __LINEFOLLOW_CXX__
#define __LINEFOLLOW_CXX__

#include "LineFollow.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <sstream>

#include <stdexcept>

namespace llcv {
  
  LineFollow::LineFollow() : llcv_base("LineFollow") {
    this->set_verbosity((msg::Level_t)0);
    _thickness = 2;
    _radius = 15;
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
    // Move along the line
    //

    float radius = _radius;
    float min_angle = 0;
    float _angle_tol = 20;
    float _dist_tol = 10;

    geo2d::Vector<float> center_pt = init_pt;
    geo2d::Vector<float> prev_pt;
    geo2d::Circle<float> circle;
    int ctr = 0;

    bool first = true;
    std::vector<float> radius_v;
    _radius_v = {8,10,12,14,16};
    
    while(1) {
      ctr+=1;
      LLCV_DEBUG() << "@ctr=" << ctr << std::endl;
      //
      // get distance to edge
      //
      auto edge_dist = DistanceToEdge(center_pt);

      if (edge_dist < _radius)
	radius = edge_dist - 1;
      else
	radius = _radius;
      
      circle.center = center_pt;
      circle.radius = radius;

      auto pt_v = larocv::OnCircleGroups(_img,circle);
      //auto pt_vv = larocv::OneCircleGroupsOnCircleArray(_img,circle.center,radius_v);
      if (pt_v.size() == 1) break;
      
      //
      // choose the comparison point
      //
      
      if (first) {
	prev_pt = geo2d::mean(start,circle.center);
	first = false;
      } 

      size_t mpid2 = larocv::kINVALID_SIZE;
      float min_dist = larocv::kINVALID_FLOAT;
      for(size_t pid=0; pid<pt_v.size(); ++pid) {
	float distance = geo2d::dist(pt_v[pid],prev_pt);
	if (distance < min_dist) {
	  min_dist = distance;
	  mpid2 = pid;
	}
      }
      
      if (min_dist > _dist_tol) {
	LLCV_DEBUG() << "exit min_dist=" << min_dist << " _dist_tol=" << _dist_tol << std::endl;
	break;
      }
      
      geo2d::Vector<float> pt2 = pt_v[mpid2];
      
      geo2d::Line<float> line2(pt2, pt2 - circle.center);
	
      //
      // minimize angle between crossing points (closest to 180)
      //
      min_angle = larocv::kINVALID_FLOAT;

      size_t mpid1 = larocv::kINVALID_SIZE;

      for(size_t pid=0; pid < pt_v.size(); ++pid) {
	if (pid==mpid2) continue;
	const auto& pt1 = pt_v[pid];
	auto line1 = geo2d::Line<float>(pt1, pt1 - circle.center);
	float angle = std::fabs(geo2d::angle(line1) - geo2d::angle(line2));
	if (angle > 90) angle = std::fabs(180 - angle);
	LLCV_DEBUG() << "(" << pid << "," << mpid2 << ") (" << pt1.x << "," << pt1.y << ")&(" << pt2.x << "," << pt2.y << ") a=" << angle << std::endl;
	if (angle < min_angle) {
	  min_angle = angle;
	  mpid1 = pid;
	  LLCV_DEBUG() << "...accepted (" << min_angle << "," << mpid1 << ")" << std::endl;
	}
      }
      
      if (min_angle > _angle_tol) {
	LLCV_DEBUG() << "exit angle" << std::endl;
	break;
      }

      ret_v.emplace_back(AsLine(circle.center,pt_v[mpid1]));

      prev_pt   = circle.center;
      center_pt = pt_v[mpid1];

      if (ctr>100) break;
    }
    
    cv::imwrite("test.png",_img);

    LLCV_DEBUG() << "end" << std::endl;
    return ret_v;
  }

  bool LineFollow::InitializeFirstPoint(geo2d::Vector<float> start, geo2d::Vector<float>& init_pt) {
    bool exists = false;

    // define the small image
    float pad_x = 20;
    float pad_y = 20;
    
    float min_x = start.x;
    float min_y = start.y;
    
    min_x -= pad_x;
    min_y -= pad_y;
    
    float dx = 2*pad_x;
    float dy = 2*pad_y;

    LLCV_DEBUG() << "start=(" << start.x << "," << start.y << ") : m=(" << min_x << "," << min_y << ") : d=(" << dx << "," << dy << ")" << std::endl;
    auto small_img = larocv::SmallImg(_img,geo2d::Vector<float>(min_x,min_y),dx,dy);
    cv::imwrite("test0a.png",small_img);

    float ddx = min_x-dx;
    float ddy = min_y-dy;

    int ecase = -1;
    bool repo1 = false;
    bool repo2 = false;

    if (start.x  < (_img.cols-1) and start.y == 0) {
      
      if (start.x > (_img.rows - 1 - dx)) {
      	start.x *= -1;
      	start.x += (_img.rows - 1);
	start.x *= -1;
	start.x += 2*dx;
      	repo2 = true;
      }

      if (!repo2 and start.x > 1.5*dx){
	start.x -= ddx;
	repo1 = true;
      }
      
      ecase = 0;
    }
    else if (start.x < (_img.cols-1) and start.y >= (_img.rows-1)) {

      if (start.x > (_img.rows - 1 - dx)) {
      	start.x *= -1;
      	start.x += (_img.rows - 1);
	start.x *= -1;
	start.x += 2*dx;
      	repo2 = true;
      }

      if (!repo2 and start.x > 1.5*dx) {
	start.x -= ddx;
	repo1 = true;
      }

      start.y  = small_img.rows-1;

      ecase = 1;
    }
    else if (start.x == 0 and start.y < (_img.rows-1)) {

      if (start.y > (_img.rows - 1 - dy)) {
	start.y *= -1;
	start.y += (_img.rows - 1);
	start.y *= -1;
	start.y += 2*dy;
	repo2 = true;
      }

      if (!repo2 and start.y > 1.5*dy) {
	start.y -= ddy;
	repo1 = true;
      }

      ecase = 2;
    }
    else if (start.x >= (_img.cols-1) and start.y < (_img.rows-1)) {

      start.x  = small_img.cols-1;

      if (start.y > (_img.rows - 1 - dy)) {
	start.y *= -1;
	start.y += (_img.rows - 1);
	start.y *= -1;
	start.y += 2*dy;
	repo2 = true;
      }
      
      if (!repo2 and start.y > 1.5*dy) {
	start.y -= ddy;
	repo1 = true;
      }

      ecase = 3;
    }

    
    else {
      throw std::runtime_error("unhandled case");
    }

    LLCV_DEBUG() << "ecase=" << ecase << " repo1=" << repo1 << " repo2=" << repo2 << std::endl;

    // define the circle
    LLCV_DEBUG() << "ddx: " << ddx << " ddy: " << ddy << std::endl;
    LLCV_DEBUG() << "small start=(" << start.x << "," << start.y << ")" << std::endl;
    auto small_cimg = larocv::BlankImage(small_img,0);
    cv::circle(small_cimg,cv::Point((int)(start.x+0.5),(int)(start.y+0.5)),(int)20,cv::Scalar(255),1);
    cv::imwrite("test0b.png",small_cimg);

    auto nz_pt_v = larocv::FindNonZero(small_cimg);
    
    auto small_bimg = larocv::BlankImage(small_cimg,0);
    auto small_wimg = larocv::BlankImage(small_cimg,255);

    float thickness = 3;

    float n_line = -1.0*larocv::kINVALID_FLOAT;

    geo2d::Vector<float> line_pt;

    for(auto& nz_pt : nz_pt_v) {
      
      small_bimg.setTo(cv::Scalar(0));
      small_wimg.setTo(cv::Scalar(255));

      cv::line(small_bimg,
	       cv::Point((int)(start.x + 0.5),(int)(start.y+0.5)),
	       cv::Point((int)(nz_pt.x + 0.5),(int)(nz_pt.y+0.5)),
	       cv::Scalar(255),
	       thickness);
      
      auto line_ctor_v = larocv::FindContours(small_bimg);
      const auto& line_ctor = line_ctor_v.front();
      
      auto line_mask = larocv::MaskImage(small_img,line_ctor,-1,false);

      float n_line_mask = larocv::CountNonZero(line_mask);
      
      if (n_line_mask > n_line) {
	n_line = n_line_mask;
      	line_pt = nz_pt;
      }
      
    }

    LLCV_DEBUG() << "line_pt=(" << line_pt.x << "," << line_pt.y << ") n_line=" << n_line << std::endl;
    
    init_pt = line_pt;

    switch(ecase) {

    case 0 : {
      if (repo1)
	init_pt.x += (ddx);

      if (repo2) {
	init_pt.x -= 2*dx;
	init_pt.x *= -1;
      	init_pt.x -= (_img.rows - 1);
      	init_pt.x *= -1;
      }

      break;
    }

    case 1: {
      if (repo1)
	init_pt.x += (ddx);

      if (repo2) {
	init_pt.x -= 2*dx;
	init_pt.x *= -1;
      	init_pt.x -= (_img.rows - 1);
      	init_pt.x *= -1;
      }

      init_pt.y  = _img.rows - (small_img.rows - init_pt.y) - 1;
      break;
    }

    case 2: {
      if (repo1)
	init_pt.y += (ddy);
      if (repo2) {
	init_pt.y -= 2*dy;
	init_pt.y *= -1;
	init_pt.y -= (_img.rows - 1);
	init_pt.y *= -1;
      }
      break;
    }

    case 3: {
      init_pt.x  = _img.cols - (small_img.cols - init_pt.x) - 1;
      if (repo1)
	init_pt.y += (ddy);
      if (repo2) {
	init_pt.y -= 2*dy;
	init_pt.y *= -1;
	init_pt.y -= (_img.rows - 1);
	init_pt.y *= -1;
      }
      break;
    }
      
    default:  {
      break;
    }
      
    }
    
    LLCV_DEBUG() << "init_pt=(" << init_pt.x << "," << init_pt.y << ")" << std::endl;

    return exists;
  }

  larocv::GEO2D_Contour_t LineFollow::AsLine(geo2d::Vector<float> pt1, geo2d::Vector<float> pt2) {
    larocv::GEO2D_Contour_t ret;
    
    float pad_x = 30;
    float pad_y = 30;

    float min_x = std::min(pt1.x,pt2.x);
    float min_y = std::min(pt1.y,pt2.y);

    float dx = std::abs(pt1.x - pt2.x)/2.0;
    float dy = std::abs(pt1.y - pt2.y)/2.0;

    dx += pad_x;
    dy += pad_y;

    auto small_mat = cv::Mat(dx,dy,_black_img.type(),cv::Scalar(0));

    float ox = min_x - pad_x/5;
    float oy = min_y - pad_y/5;

    pt1.x -= ox;
    pt1.y -= oy;
    
    pt2.x -= ox;
    pt2.y -= oy;

    float thickness = 3;
    cv::line(small_mat,
    	     cv::Point((int)(pt1.x+0.5),(int)(pt1.y+0.5)),
	     cv::Point((int)(pt2.x+0.5),(int)(pt2.y+0.5)),
	     //cv::Scalar(255),
	     cv::Scalar(150),
	     thickness);

    cv::imwrite("test1.png",small_mat);
    
    auto line_ctor_v = larocv::FindContours(small_mat);

    if (!line_ctor_v.empty()) {
      const auto& line_ctor = line_ctor_v.front();
      for(size_t lid=0; lid<line_ctor.size(); ++lid) {
	geo2d::Vector<int> cpt;
	cpt.x = line_ctor[lid].x;
	cpt.y = line_ctor[lid].y;
	
	cpt.x += ox;
	cpt.y += oy;

	ret.emplace_back(std::move(cpt));
      }
    }

    return ret;
  }
  
  larocv::GEO2D_Contour_t LineFollow::EdgePoints() {
    larocv::GEO2D_Contour_t ret;
    
    _black_img.setTo(0);

    //
    // edge mask
    //
    cv::line(_black_img,
	     cv::Point(0,0),
	     cv::Point(_black_img.rows-1,0),
	     cv::Scalar(255),
	     1);

    cv::line(_black_img,
	     cv::Point(0,0),
	     cv::Point(0,_black_img.cols-1),
	     cv::Scalar(255),
	     1);
    
    cv::line(_black_img,
	     cv::Point(_black_img.cols-1,0),
	     cv::Point(_black_img.rows-1,_black_img.cols-1),
	     cv::Scalar(255),
	     1);
    
    cv::line(_black_img,
	     cv::Point(0,_black_img.cols-1),
	     cv::Point(_black_img.rows-1,_black_img.cols-1),
	     cv::Scalar(255),
	     1);

    _white_img.setTo(0);
    _img.copyTo(_white_img,_black_img);

    //
    // find edge points
    //
    
    auto ctor_v = larocv::FindContours(_white_img);
    ret.reserve(ctor_v.size());
    for(const auto& ctor : ctor_v) {
      auto nz_pt_v = larocv::FindNonZero(larocv::MaskImage(_white_img,ctor,-1,false));
      // calc the average
      float mean_x = 0;
      float mean_y = 0;
      for(const auto& nz_pt : nz_pt_v) {
	mean_x += nz_pt.x;
	mean_y += nz_pt.y;
      }
      mean_x /= (float)nz_pt_v.size();
      mean_y /= (float)nz_pt_v.size();

      ret.emplace_back((int)mean_x+0.5,(int)mean_y+0.5);
    }

    return ret;
  }

  float LineFollow::DistanceToEdge(const geo2d::Vector<float>& pt) const {
    float ret = larocv::kINVALID_FLOAT;
    
    ret = std::min(ret,(float)pt.x);
    ret = std::min(ret,(float)pt.y);
    ret = std::min(ret,(float)_img.rows - (float)pt.x);
    ret = std::min(ret,(float)_img.cols - (float)pt.y);
    
    return ret;
  }



}

#endif


