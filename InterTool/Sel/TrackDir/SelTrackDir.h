#ifndef __SELTRACKDIR_H__
#define __SELTRACKDIR_H__

#include "InterTool_Core/InterSelBase.h"
#include "InterTool_Util/TruncMean.h"
namespace llcv {
  
  class SelTrackDir : public InterSelBase { 

  public:

  SelTrackDir(std::string name="SelTrackDir") : 
    InterSelBase(name), fouttree(nullptr) {}

    ~SelTrackDir(){}
    
    void Configure (const larcv::PSet &pset);
    void Initialize();
    double Select();
    void Finalize();
    
  private:

    TruncMean _TruncMean;

    TTree* fouttree;
    float fmax_hit_radius;
    size_t fplane;
    size_t fedge_length;
    float ftsigma;

    std::vector<std::vector<float> > trk_dedx_vv;
    std::vector<std::vector<float> > trk_tdedx_vv;
    std::vector<std::vector<float> > trk_range_vv;

    std::vector<float> trk_length_v;
    std::vector<int> trk_npts_v;

    //
    // overall
    //
    std::vector<float> trk_mean_dedx_v;
    std::vector<float> trk_slope_dedx_v;
    std::vector<float> trk_median_dedx_v;

    std::vector<float> trk_tmean_dedx_v;
    std::vector<float> trk_tslope_dedx_v;
    std::vector<float> trk_tmedian_dedx_v;

    //
    // start
    //
    std::vector<float> trk_start_mean_dedx_v;
    std::vector<float> trk_start_slope_dedx_v;
    std::vector<float> trk_start_median_dedx_v;

    std::vector<float> trk_start_tmean_dedx_v;
    std::vector<float> trk_start_tslope_dedx_v;
    std::vector<float> trk_start_tmedian_dedx_v;

    //
    // middle 
    //
    std::vector<float> trk_middle_mean_dedx_v;
    std::vector<float> trk_middle_slope_dedx_v;
    std::vector<float> trk_middle_median_dedx_v;

    std::vector<float> trk_middle_tmean_dedx_v;
    std::vector<float> trk_middle_tslope_dedx_v;
    std::vector<float> trk_middle_tmedian_dedx_v;
    
    //
    // end 
    //
    std::vector<float> trk_end_mean_dedx_v;
    std::vector<float> trk_end_slope_dedx_v;
    std::vector<float> trk_end_median_dedx_v;

    std::vector<float> trk_end_tmean_dedx_v;
    std::vector<float> trk_end_tslope_dedx_v;
    std::vector<float> trk_end_tmedian_dedx_v;

    void ResetTree();
    void ResizeTree(size_t ntracks);
  };

}

#endif
