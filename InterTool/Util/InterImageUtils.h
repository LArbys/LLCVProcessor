#ifndef __INTERIMAGEUTILS_H__
#define __INTERIMAGEUTILS_H__

#include "DataFormat/ImageMeta.h"

#include "LArOpenCV/Core/ImageMeta.h"

namespace llcv {

  cv::Mat As8UC3(const cv::Mat& img,int channel=-1);

  void ProjectImage2D(const larcv::ImageMeta& meta,
		      double x,double y,double z,
		      int& xx, int& yy);

  void ProjectMat(const larcv::ImageMeta& meta,
		  double x,double y,double z,
		  int& xx, int& yy);
  
}


#endif
