#ifndef __SELAHO_H__
#define __SELAHO_H__

#include "InterTool_Core/InterSelBase.h"

namespace llcv {
  
  class SelAho : public InterSelBase { 

  public:

  SelAho(std::string name="SelAho") : InterSelBase(name) {}
    ~SelAho(){}
    
    void Configure (const larcv::PSet &pset);
    void Initialize() {}
    double Select();
    void Finalize();
    
    
  protected:
    
    
  };

}


#endif
