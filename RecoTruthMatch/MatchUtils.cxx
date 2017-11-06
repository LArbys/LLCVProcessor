#ifndef MATCHUTILS_CXX
#define MATCHUTILS_CXX

#include "MatchUtils.h"
#include "LArUtil/GeometryHelper.h"
#include "LArUtil/LArProperties.h"
#include <numeric>

namespace llcv {
  
  void
  Project3D(const larcv::ImageMeta& meta,
	    double parent_x,
	    double parent_y,
	    double parent_z,
	    double parent_t,
	    uint plane,
	    double& xpixel, double& ypixel) 
  {
    
    auto geohelp = larutil::GeometryHelper::GetME();
    auto larpro  = larutil::LArProperties::GetME();

    auto vtx_2d = geohelp->Point_3Dto2D(parent_x, parent_y, parent_z, plane );
    
    double x_compression  = (double)meta.width()  / (double)meta.cols();
    double y_compression  = (double)meta.height() / (double)meta.rows();
    xpixel = (vtx_2d.w/geohelp->WireToCm() - meta.tl().x) / x_compression;
    ypixel = (((parent_x/larpro->DriftVelocity() + parent_t/1000.)*2+3200)-meta.br().y)/y_compression;
  }
  
  void
  mask_image(larcv::Image2D& target, const larcv::Image2D& ref)
  {
    if(target.meta() != ref.meta()) 
      throw larcv::larbys("Cannot mask images w/ different meta");

    auto meta = target.meta();
    std::vector<float> data = target.move();
    auto const& ref_vec = ref.as_vector();

    for(size_t i=0; i<data.size(); ++i) { if(ref_vec[i]>0) data[i]=0; }	

    target.move(std::move(data));
  }
  
}

#endif
