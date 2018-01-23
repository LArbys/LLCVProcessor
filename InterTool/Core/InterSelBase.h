#ifndef __INTERSELBASE_H__
#define __INTERSELBASE_H__

#include "LLCVBase/llcv_base.h"

namespace llcv {
  
  class InterSelBase : public llcv_base { 
  public:
  InterSelBase(std::string name="InterSelBase") : llcv_base(name) {}
    virtual ~InterSelBase(){}
    
  };

}


#endif
