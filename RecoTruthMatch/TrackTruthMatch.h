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

    
  };

}

#endif
