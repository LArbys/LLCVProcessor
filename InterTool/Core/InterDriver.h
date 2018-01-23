#ifndef __INTERDRIVER_H__
#define __INTERDRIVER_H__


#include "InterAnaBase.h"
#include "InterSelBase.h"

#include "InterTTreeManager.h"
#include "InterDataManager.h"
#include "InterImageManager.h"

namespace llcv {

  class InterDriver : public llcv_base {
  public:
  InterDriver(std::string name="InterDriver") : llcv_base(name) {}
    ~InterDriver() {}
    
  private:
    std::vector<InterAnaBase*> _ana_base_v;
    std::vector<InterSelBase*> _sel_base_v;
    
    
    InterTTreeManager _tree_mgr;
    InterDataManager  _data_mgr;
    InterImageManager _img_mgr;

  };
  
  
}

#endif
