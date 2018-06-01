#ifndef __COSMICTAG_H__
#define __COSMICTAG_H__

#include "LineFollow.h"

namespace llcv {

  class CosmicTag : public llcv_base {

  public:
    CosmicTag();
    ~CosmicTag() {}

    void Reset();

    void TagCosmic(const cv::Mat& img, const cv::Mat& dimg);
      
    void DrawContours(cv::Mat& mat3d) const;
    void DrawLines(cv::Mat& mat3d) const;
    
    // storage for cosmic contour
    const std::vector<larocv::GEO2D_Contour_t>& CosmicContours() const { return _cosmic_ctor_v; }
    
    // distance between point and nearest cosmic
    size_t NearestCosmicToPoint(const cv::Point_<int>& pt, float& distance) const;
    size_t NearestCosmicToContour(const larocv::GEO2D_Contour_t& ctor, float& distance) const;

  private:
    
    larocv::GEO2D_Contour_t LinesToContour(const larocv::GEO2D_ContourArray_t& ctor_v);
    

  private:

    LineFollow _LineFollow;

    // storage for cosmic contour
    std::vector<larocv::GEO2D_Contour_t> _cosmic_ctor_v;

    // storage for individual lines
    std::vector<std::vector<larocv::GEO2D_Contour_t> > _cosmic_line_ctor_vv;

  private:

  };


}

#endif
