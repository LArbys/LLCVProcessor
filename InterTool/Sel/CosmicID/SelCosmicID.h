#ifndef __SELCOSMICID_H__
#define __SELCOSMICID_H__

#include "InterTool_Core/InterSelBase.h"

namespace llcv {
  
  class SelCosmicID : public InterSelBase { 

  public:

  SelCosmicID(std::string name="SelCosmicID") : InterSelBase(name) {}
    ~SelCosmicID(){}
    
    void Configure (const larcv::PSet &pset);
    void Initialize() {}
    double Select();
    void Finalize();
    
    
  protected:
    
    
  };

}


#endif
