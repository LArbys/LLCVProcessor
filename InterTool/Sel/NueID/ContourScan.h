#ifndef __CONTOURSCAN_H__
#define __CONTOURSCAN_H__

#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"

#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

#include <array>

namespace llcv {

  class ContourScan {

  public:
    ContourScan() {
      _geo._num_planes = 3;
      _geo._trigger_tick = 3200;
      _geo._xplane_tick_resolution = 2;

      _plane_comb_v[0] = {{0,1,2}};
      _plane_comb_v[1] = {{0,2,1}};
      _plane_comb_v[2] = {{1,2,0}};
    }
    
    ~ContourScan() {}

    void SetPlaneInfo(const larocv::ImageMeta& meta)
    { _geo.ResetPlaneInfo(meta); }
    
    void Reset();
    void RegisterContour(const cv::Mat& img, 
			 const larocv::GEO2D_Contour_t& ctor,
			 const size_t plane,
			 const float rad);

    void AddPixels(const larocv::GEO2D_Contour_t& pt_v,
		   const size_t plane);
    
    void Scan(const std::array<cv::Mat,3>& in_img_v,
	      const std::array<cv::Mat,3>& dead_img_v,
	      std::array<cv::Mat,3>& out_img_v);
	      
    
  private:
    
    larocv::LArPlaneGeo _geo;

    std::array<larocv::GEO2D_Contour_t,3> _ctor_v;

    std::array<std::array<int,3>,3> _plane_comb_v;
    
  };



}

#endif
