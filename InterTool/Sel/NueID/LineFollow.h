#ifndef __LINEFOLLOW_H__
#define __LINEFOLLOW_H__

#include "LLCVBase/llcv_base.h"

#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

namespace llcv {

  class LineFollow : public llcv_base {

  public:
    LineFollow();
    ~LineFollow() {}

    larocv::GEO2D_ContourArray_t FollowEdgeLine(const geo2d::Vector<float>& start);

    void SetImageDimension(const cv::Mat& img);

    larocv::GEO2D_Contour_t EdgePoints();


  private:

    cv::Mat _img;
    cv::Mat _black_img;
    cv::Mat _white_img;
    
    int _thickness;
    float _radius;
    std::vector<float> _radius_v;

  private:
    bool InitializeFirstPoint(geo2d::Vector<float> start, geo2d::Vector<float>& init_pt);
    larocv::GEO2D_Contour_t AsLine(geo2d::Vector<float> pt1, geo2d::Vector<float> pt2);
    float DistanceToEdge(const geo2d::Vector<float>& pt) const;
    
  };

}

#endif
