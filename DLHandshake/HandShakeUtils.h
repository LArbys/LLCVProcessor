#ifndef __HANDSHAKEUTILS_H__
#define __HANDSHAKEUTILS_H__

#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include "DataFormat/EventPixel2D.h"

namespace llcv {

  larocv::GEO2D_Contour_t as_contour(const larcv::Pixel2DCluster& ctor);
  
  larocv::GEO2D_ContourArray_t as_contour_array(const std::vector<larcv::Pixel2DCluster>& ctor_v);

}
#endif
