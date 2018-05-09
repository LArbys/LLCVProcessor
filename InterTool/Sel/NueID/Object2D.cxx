#ifndef __OBJECT2D_CXX__
#define __OBJECT2D_CXX__

#include "Object2D.h"

#include <stdexcept>

namespace llcv {

  bool Object2DCollection::HasObject(size_t plane) const {
    bool res = false;

    for(const auto& obj : (*this))
      if (obj._plane == plane) 
	res = true;

    return res;
  }
  const Object2D& Object2DCollection::PlaneObject(size_t plane) const {
    size_t res = larocv::kINVALID_SIZE;
    for(size_t oid=0; oid < this->size(); ++oid)
      if ((*this)[oid]._plane == plane)
	res = oid;
    
    if (res == larocv::kINVALID_SIZE) 
      throw std::runtime_error("Check plane exists with HasObject");

    return (*this)[res];
  }
  
}

#endif
