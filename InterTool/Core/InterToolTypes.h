#ifndef __INTERTOOLTYPES_H__
#define __INTERTOOLTYPES_H__

#include <string>

namespace llcv {

  enum InterImageType {
    kINTER_IMAGE_TYPE_UNKNOWN = 0,
    kADC = 1,
    kTRK = 2,
    kSHR = 3,
    kINTER_IMAGE_TYPE_MAX = 4
  };

  enum InterSpecType {
    kINTER_SPEC_TYPE_UNKNOWN = 0,
    kINT     = 1,
    kFLOAT   = 2,
    kDOUBLE  = 3,
    kVFLOAT  = 4,
    kVVFLOAT = 5,
    KINTER_SPEC_TYPE_MAX = 6
  };

  InterSpecType LeafToSpecType(const std::string& type);

}


#endif
