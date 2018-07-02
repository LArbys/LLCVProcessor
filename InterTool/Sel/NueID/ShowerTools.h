#ifndef __SHOWERTOOLS_H__
#define __SHOWERTOOLS_H__

#include "LLCVBase/llcv_base.h"

#include "Object2D.h"
#include "DataFormat/Image2D.h"

#include <array>

namespace llcv {
  
  class ShowerTools : public llcv_base {
  
  public:
    
  ShowerTools() : llcv_base("ShowerTools") {}
    ~ShowerTools() {}
    
    void ReconstructAngle(const std::vector<larcv::Image2D*>& img_v,
			  const std::array<cv::Mat,3>& aimg_v, 
			  Object2DCollection& obj_col);
    
    void ReconstructLength(const std::vector<larcv::Image2D*>& img_v,
			   const std::array<cv::Mat,3>& aimg_v,
			   Object2DCollection& obj_col);

    void ReconstructdQdx(const std::vector<larcv::Image2D*>& img_v,
			 const std::array<cv::Mat,3>& aimg_v,
			 Object2DCollection& obj_col,
			 double dtrunk);
    

  };


}

#endif
