#ifndef __SKELETONIZE_H__
#define __SKELETONIZE_H__

#include "VThinning.h"

namespace llcv {

  class Skeletonize {
  public:
    Skeletonize(){}
    ~Skeletonize() {}
    
  private:
    VThinning _VThinning;
    

  };

}


#endif
