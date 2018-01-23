#ifndef __INTERDRIVER_H__
#define __INTERDRIVER_H__

#include "InterAnaBase.h"
#include "InterSelBase.h"

namespace llcv {

  class InterDriver : public llcv_base {
  public:
  InterDriver(std::string name="InterDriver") : llcv_base(name) {}
    ~InterDriver() {}
    
  private:
    std::vector<InterAnaBase*> _ana_base_v;
    std::vector<InterSelBase*> _sel_base_v;
    
    
  };
  
  
}

#endif
