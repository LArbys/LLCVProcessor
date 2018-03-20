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
    
    std::vector<std::vector<float> > trk_dedx_vv;
    std::vector<std::vector<float> > trk_dedx_lin_vv;

    /* std::vector<float> trk_avg_dedx_v; */
    /* std::vector<float> trk_tavg_dedx_v; */
    /* std::vector<float> trk_med_dedx_v; */
    /* std::vector<float> trk_tmed_dedx_v; */
    
    void ResetTree();

  };

}

#endif
