#ifndef __SELTRACKSCATTER_H__
#define __SELTRACKSCATTER_H__

#include "InterTool_Core/InterSelBase.h"
#include "LArOpenCV/ImageCluster/AlgoClass/DefectBreaker.h"
#include "LArOpenCV/ImageCluster/AlgoClass/PixelScan3D.h"

namespace llcv {
  
  class SelTrackScatter : public InterSelBase { 

  public:

  SelTrackScatter(std::string name="SelTrackScatter") : InterSelBase(name), _outtree(nullptr) {}
    ~SelTrackScatter(){}
    
    void Configure (const larcv::PSet &pset);
    void Initialize();
    double Select();
    void Finalize();
    
  private:

    TTree* _outtree;
    
    std::vector<std::vector<float> > _track_x_vv;
    std::vector<std::vector<float> > _track_y_vv;
    std::vector<std::vector<float> > _track_z_vv;

  private:

    size_t _cropx;
    size_t _cropy;

    larocv::DefectBreaker _LR_DefectBreaker;
    larocv::DefectBreaker _SR_DefectBreaker;

    larocv::PixelScan3D _PixelScan3D;

    larocv::GEO2D_ContourArray_t FindAndMaskVertex(const cv::Mat& mat,const cv::Point_<int> vertex);
    larocv::GEO2D_ContourArray_t FindAndBreakVertex(const cv::Mat& mat,const cv::Point_<int> vertex);
    
    std::vector<std::vector<size_t> > AssociateToTracks(const larocv::GEO2D_ContourArray_t& ctor_v,
							const std::vector<larocv::GEO2D_ContourArray_t>& track_ctor_vv,
							const size_t plane);

    bool ContainsTrack(const std::vector<size_t>& tin_v);
    float TrackFraction(const std::vector<size_t>& tin_v, size_t tid);
    std::pair<float,float> TrackAngle(const larlite::track& track); 
    float TrackLength(const larlite::track& track);

  };

}


#endif
