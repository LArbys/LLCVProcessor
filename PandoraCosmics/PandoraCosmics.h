#ifndef PANDORACOSMICS_H
#define PANDORACOSMICS_H

#include "LLCVBase/AnaBase.h"

namespace llcv {
  
  class PandoraCosmics : public AnaBase {
    
  public:
    
  PandoraCosmics(const std::string name="PandoraCosmics") : AnaBase(name) {}
    ~PandoraCosmics() {}

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
