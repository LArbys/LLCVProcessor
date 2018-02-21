#ifndef __INTERIMAGEUTILS_H__
#define __INTERIMAGEUTILS_H__

//lcv
#include "DataFormat/ImageMeta.h"
#include "DataFormat/EventPixel2D.h"

// locv
#include "LArOpenCV/Core/ImageMeta.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"

namespace llcv {

  cv::Mat As8UC3(const cv::Mat& img, int channel=-1);

  void ProjectImage2D(const larcv::ImageMeta& meta,
		      double x,double y,double z,
		      int& xx, int& yy);

  void ProjectMat(const larcv::ImageMeta& meta,
		  double x,double y,double z,
		  int& xx, int& yy);

  larocv::GEO2D_Contour_t AsContour(const larcv::Pixel2DCluster& pcluster, const larocv::ImageMeta& meta);
  
}


#endif
