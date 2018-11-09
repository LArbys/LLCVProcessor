#ifndef PLAYAROUND_H
#define PLAYAROUND_H

#include "LLCVBase/AnaBase.h"

namespace llcv {
  
  class PlayAround : public AnaBase {
    
  public:
    
  PlayAround(const std::string name="PlayAround") : AnaBase(name) {}
    ~PlayAround() {}

    void configure(const larcv::PSet& cfg);
    void initialize();
    bool process(larcv::IOManager& mgr, larlite::storage_manager& sto);
    void finalize();

  private:


    TTree* _tree;
    
    int _run;
    int _subrun;
    int _event;

  };

}

#endif
