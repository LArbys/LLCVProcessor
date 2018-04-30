#ifndef __SELTRIANGLESTUDY_H__
#define __SELTRIANGLESTUDY_H__

#include "InterTool_Core/InterSelBase.h"
#include "LArOpenCV/ImageCluster/AlgoClass/PixelScan3D.h"
#include "TStopwatch.h"
#include <array>

namespace llcv {

  class SelTriangleStudy : public InterSelBase { 

  public:

  SelTriangleStudy(std::string name="SelTriangleStudy") : InterSelBase(name), _outtree(nullptr) {}
    ~SelTriangleStudy(){}
    
    void Configure (const larcv::PSet &pset);
    void Initialize();
    double Select();
    void Finalize();
    
  private:
    TTree* _outtree;

    //
    // Members
    //
  private:

    size_t _cropx;
    size_t _cropy;

    larocv::PixelScan3D _PixelScan3D;

    TStopwatch _twatch;

    void ResizeOutput(size_t sz);

    std::pair<float,float> ShowerAngle(const larlite::shower& shower);
    void FillNeighbors(const std::array<cv::Mat,3>& parent_v,
		       std::array<cv::Mat,3>& child_v) const;

    larocv::GEO2D_Contour_t Neighbors(const geo2d::Vector<int>& pt, const cv::Mat& mat) const;

    //
    // TTree
    //
  private:

    std::vector<float> _shower_x_v;
    std::vector<float> _shower_y_v;
    std::vector<float> _shower_z_v;

  };

}


#endif
