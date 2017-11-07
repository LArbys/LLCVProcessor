#ifndef TRACKTRUTHMATCH_H
#define TRACKTRUTHMATCH_H

#include "LLCVBase/AnaBase.h"

namespace llcv {
  
  class TrackTruthMatch : public AnaBase {
    
  public:
    
  TrackTruthMatch(const std::string name="TrackTruthMatch") : AnaBase(name) {}
    ~TrackTruthMatch() {}

    void configure(const larcv::PSet&);
    void initialize();
    bool process(larcv::IOManager& mgr, larlite::storage_manager& sto);
    void finalize();

  private:
    TTree* _tree;
    
    int _run;
    int _subrun;
    int _event;
    int _vtxid;
    int _ntracks;
    std::vector<int> _npts_v;
    std::vector<std::vector<int> > _pt_type_vv;

    void ClearVertex();
      
  };

}

#endif
