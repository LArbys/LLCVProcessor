#ifndef __INTERSELTOY_H__
#define __INTERSELTOY_H__

#include "InterTool_Core/InterSelBase.h"

namespace llcv {
  
  class InterSelToy : public llcv_base { 

  public:

  InterSelToy(std::string name="InterSelToy") : llcv_base(name) {}
    virtual ~InterSelToy(){}
    
    void Configure (const larcv::PSet &pset) {}
    float Select() { return 0.0; }
    void Finalize() {}
    
    
  protected:
    
    
  };

}


#endif
