#ifndef __OBJECT2D_CXX__
#define __OBJECT2D_CXX__

#include "Object2D.h"

#include <stdexcept>

namespace llcv {

  bool Object2DCollection::HasObject(size_t plane) const {
    bool res = false;

    for(const auto& obj : (*this))
      if (obj._plane == plane) 
	res = true;

    return res;
  }
  const Object2D& Object2DCollection::PlaneObject(size_t plane) const {
    size_t res = larocv::kINVALID_SIZE;
    for(size_t oid=0; oid < this->size(); ++oid)
      if ((*this)[oid]._plane == plane)
	res = oid;
    
    if (res == larocv::kINVALID_SIZE) 
      throw std::runtime_error("Check plane exists with HasObject");

    return (*this)[res];
  }
  
  Object2D& Object2DCollection::PlaneObjectRW(size_t plane) {
    size_t res = larocv::kINVALID_SIZE;
    for(size_t oid=0; oid < this->size(); ++oid)
      if ((*this)[oid]._plane == plane)
	res = oid;
    
    if (res == larocv::kINVALID_SIZE) 
      throw std::runtime_error("Check plane exists with HasObject");

    return (*this)[res];
  }
  

  std::vector<int> Object2DCollection::Planes() const {
    std::vector<int> res_v(3,0);
    
    for(size_t plane=0; plane<3; ++plane) {
      if(!HasObject(plane)) continue;
      res_v[plane] = 1;
    }
    
    return res_v;
  }

  std::vector<int> Object2DCollection::XDead(const std::array<cv::Mat,3>& dimg_v,
					     const cv::Mat& white_img,
					     float radius) const {
    std::vector<int> res_v(3,0);
    
    for(size_t plane=0; plane<3; ++plane) {
      if (!HasObject(plane)) continue;

      const auto obj2d = PlaneObject(plane);

      auto& xdead = res_v[plane];
      const auto& dimg = dimg_v[plane];
      
      int nz_white = larocv::CountNonZero(larocv::MaskImage(white_img,obj2d.Line(),-1,false));
      int nz_dead  = larocv::CountNonZero(larocv::MaskImage(dimg,obj2d.Line(),-1,false));
      if (nz_white != nz_dead) xdead = 1;
      if (xdead == 1) continue;
      
      //does this line lie @ dead region boundary;
      geo2d::Vector<float> edge1,edge2;
      larocv::FindEdges(obj2d.Line(),edge1,edge2);

      // check edge1
      auto circle = geo2d::Circle<float>(edge1,radius);
      nz_white = larocv::CountNonZero(larocv::MaskImage(white_img,circle,-1,false));
      nz_dead =  larocv::CountNonZero(larocv::MaskImage(dimg,circle,-1,false));

      if (nz_white != nz_dead) xdead = 1;
      if (xdead == 1) continue;

      // check edge2
      circle = geo2d::Circle<float>(edge2,radius);
      nz_white = larocv::CountNonZero(larocv::MaskImage(white_img,circle,-1,false));
      nz_dead =  larocv::CountNonZero(larocv::MaskImage(dimg,circle,-1,false));
      if (nz_white != nz_dead) xdead = 1;
    } // end plane

    return res_v;
  }
  
  float Object2D::LinedX() const {
    float ret = larocv::kINVALID_FLOAT;
    ret = (this->Edge() - this->Start()).x;
    ret /= this->LineLength();
    return ret;
  }
  
  float Object2D::LinedY() const {
    float ret = larocv::kINVALID_FLOAT;
    ret = (this->Edge() - this->Start()).y;
    ret /= this->LineLength();
    return ret;
  }  

  float Object2D::Charge(const cv::Mat& mat) const {
    float res = 0;
    
    for(const auto& poly : _expand_polygon_v)
      res += poly.Charge(mat);
    
    return res;
  }

  float Object2D::Fraction(const cv::Mat& img1, const cv::Mat& img2) const {
    float res = 0;

    float top = 0;
    float bot = 0;

    for(const auto& poly : _expand_polygon_v) {
      top += larocv::CountNonZero(larocv::MaskImage(img1,poly.Contour(),-1,false));
      bot += larocv::CountNonZero(larocv::MaskImage(img2,poly.Contour(),-1,false));
    }

    if (bot != 0)
      res = top / bot;

    return res;
  }


}

#endif
