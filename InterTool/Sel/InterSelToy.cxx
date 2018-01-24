#ifndef __INTERSELTOY_CXX__
#define __INTERSELTOY_CXX__

#include "InterSelToy.h"

namespace llcv {

  void InterSelToy::Configure (const larcv::PSet &pset) {
    set_verbosity((msg::Level_t)pset.get<int>("Verbosity",2));
    LLCV_DEBUG() << "start" << std::endl;

    LLCV_DEBUG() << "end" << std::endl;
  }

  float InterSelToy::Select() {
    LLCV_DEBUG() << "start" << std::endl;

    LLCV_DEBUG() << "end" << std::endl;
    return 0.0;
  }
  
  void InterSelToy::Finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    
    LLCV_DEBUG() << "end" << std::endl;
  }

}


#endif
