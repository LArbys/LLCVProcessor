#ifndef __OBJECT2D_H__
#define __OBJECT2D_H__

#include "InterTool_Util/Triangle.h"
#include "InterTool_Util/Polygon.h"

#include "TVector3.h"

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
    const geo2d::Vector<float>& Start() const { return _triangle.Apex(); }
    int NPolygons() const { return (int)(_polygon_v.size()); }
    float LineFrac() const { return _line_frac; }

    const Triangle& triangle() const { return _triangle; }
    const std::vector<Polygon>& Polygons() const { return _polygon_v; }
    const larocv::GEO2D_Contour_t& Line() const { return _line; }
    size_t Plane() const { return _plane; }

  };

  class Object2DCollection : public std::vector<Object2D> {

  public:
    Object2DCollection() {}
    ~Object2DCollection() {}

    bool HasObject(size_t plane) const;
    const Object2D& PlaneObject(size_t plane) const;
    
    void SetTheta(float theta) { _theta = theta; }
    void SetPhi(float phi)     { _phi = phi;     }
    void SetStart(float x, float y, float z) 
    { _start = TVector3(x,y,z); }
    void SetLength(float length) { _length = length; };
    void SetScore(float score) { _score = score; }

    float Theta() const { return _theta; }
    float Phi()   const { return _phi; }
    const TVector3& Start() const { return _start; }
    float Length() const { return _length; }
    float Score() const { return _score; }

    float dX() const { return std::cos(_theta) * std::sin(_phi); }
    float dY() const { return std::sin(_theta);                  }
    float dZ() const { return std::cos(_theta) * std::cos(_phi); }
    
    std::vector<int> Planes() const;
    
  private:
    float _theta;
    float _phi;
    float _length;
    TVector3 _start;
    float _score;

  };

}

#endif
