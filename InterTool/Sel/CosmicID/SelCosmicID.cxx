#ifndef __SELCOSMICID_CXX__
#define __SELCOSMICID_CXX__

#include "SelCosmicID.h"

namespace llcv {

  void SelCosmicID::Configure (const larcv::PSet &pset) {
    set_verbosity((msg::Level_t)pset.get<int>("Verbosity",2));
    LLCV_DEBUG() << "start" << std::endl;

    LLCV_DEBUG() << "end" << std::endl;
  }

  double SelCosmicID::Select() {
    LLCV_DEBUG() << "start" << std::endl;
    LLCV_DEBUG() << "=======================" << std::endl;
    
    
    
    
    
    
    
    
    
    LLCV_DEBUG() << "=======================" << std::endl;
    LLCV_DEBUG() << "end" << std::endl;
    return 0.0;
  }
  
  void SelCosmicID::Finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    
    LLCV_DEBUG() << "end" << std::endl;
  }

}


#endif
