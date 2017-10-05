#ifndef __HANDSHAKEUTILS_CXX__
#define __HANDSHAKEUTILS_CXX__

#include "HandShakeUtils.h"

namespace llcv {

  larocv::GEO2D_Contour_t as_contour(const larcv::Pixel2DCluster& contour)
  {
    ::larocv::GEO2D_Contour_t ctor;
    ctor.resize(contour.size());
    for(size_t pt_idx=0; pt_idx < contour.size(); ++pt_idx) {
      auto const& pt = contour[pt_idx];
      ctor[pt_idx].x = pt.X();
      ctor[pt_idx].y = pt.Y();
    }
    return ctor;
  }
  
  larocv::GEO2D_ContourArray_t as_contour_array(const std::vector<larcv::Pixel2DCluster>& contour_v)
  {
    ::larocv::GEO2D_ContourArray_t ctor_v;
    ctor_v.reserve(contour_v.size());
    for(auto const& contour : contour_v)
      ctor_v.emplace_back(std::move(as_contour(contour)));
    return ctor_v;
  }

}

#endif
