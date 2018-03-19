#ifndef __SELTRACKDIR_H__
#define __SELTRACKDIR_H__

#include "InterTool_Core/InterSelBase.h"

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

    TTree* fouttree;
    float fmax_hit_radius;
    size_t fplane;
    
    std::vector<std::vector<float> > trk_dedx_vv;

    void ResetTree();

  };

}

#endif
