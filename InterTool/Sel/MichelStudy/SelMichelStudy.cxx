#ifndef __SELMICHELSTUDY_CXX__
#define __SELMICHELSTUDY_CXX__

#include "SelMichelStudy.h"

namespace llcv {

  void SelMichelStudy::Configure (const larcv::PSet &pset) {
    set_verbosity((msg::Level_t)pset.get<int>("Verbosity",2));
    LLCV_DEBUG() << "start" << std::endl;

    LLCV_DEBUG() << "end" << std::endl;
  }

  double SelMichelStudy::Select() {
    LLCV_DEBUG() << "start" << std::endl;
    LLCV_DEBUG() << "=======================" << std::endl;
    
    
    LLCV_DEBUG() << "=======================" << std::endl;
    LLCV_DEBUG() << "end" << std::endl;
    return 0.0;
  }
  
  void SelMichelStudy::Finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    
    LLCV_DEBUG() << "end" << std::endl;
  }

}


#endif
