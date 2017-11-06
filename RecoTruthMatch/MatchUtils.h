#ifndef MATCHUTILS_H
#define MATCHUTILS_H

#include "DataFormat/Image2D.h"

namespace llcv {

  void
  Project3D(const larcv::ImageMeta& meta,
	    double parent_x,
	    double parent_y,
	    double parent_z,
	    double parent_t,
	    uint plane,
	    double& xpixel, double& ypixel);


  void
  mask_image(larcv::Image2D& target, const larcv::Image2D& ref);
  
}
#endif


