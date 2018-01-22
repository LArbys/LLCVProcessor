#ifndef __INTERIMAGE_H__
#define __INTERIMAGE_H__

#include "LArOpenCV/Core/ImageMeta.h"
#include "DataFormat/Image2D.h"

namespace llcv {

  class InterImage {
  public:
    cv::Mat mat;
    larocv::ImageMeta meta;
    larcv::Image2D img2d;
  };

}


#endif
