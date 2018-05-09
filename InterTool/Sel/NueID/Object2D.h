#ifndef __OBJECT2D_H__
#define __OBJECT2D_H__

#include "InterTool_Util/Triangle.h"
#include "InterTool_Util/Polygon.h"

namespace llcv {

  class Object2D {
  public:
    Object2D() {}
    ~Object2D() {}
    
    Triangle _triangle;
    std::vector<Polygon> _polygon_v;
    larocv::GEO2D_Contour_t _line;
    geo2d::Vector<float> _edge;
    float _line_frac;
    size_t _plane;

  public:
    float LineLength() const { return geo2d::dist(_triangle.Apex(),_edge); }

  };

  class Object2DCollection : public std::vector<Object2D> {
  public:
    Object2DCollection() {}
    ~Object2DCollection() {}

    bool HasObject(size_t plane) const;
    const Object2D& PlaneObject(size_t plane) const;
    
  };

}

#endif
