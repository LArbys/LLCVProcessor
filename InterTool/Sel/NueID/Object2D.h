#ifndef __OBJECT2D_H__
#define __OBJECT2D_H__

#include "InterTool_Util/Triangle.h"
#include "InterTool_Util/Polygon.h"

#include "TVector3.h"

#include "DataFormat/Image2D.h"

namespace llcv {

  class Object2D {
  public:
    Object2D(){}
    ~Object2D() {}
    
    Triangle _triangle;
    Triangle _brem_triangle;

    // list of "large enough" clusters which consititue 2D particle
    std::vector<Polygon> _polygon_v; 

    // list of all clusters which lie inside expanded triangle
    std::vector<Polygon> _expand_polygon_v;

    larocv::GEO2D_Contour_t _line;
    geo2d::Vector<float> _edge;
    float _line_frac;
    size_t _plane;
    int _n_brem;

    // 3D length estimated from plane
    float _length;

    // 3D dQdx estimated from plane
    double _dqdx;

    int _brem_index;

  public:
    float LineLength() const { return geo2d::dist(_triangle.Apex(),_edge); }
    const geo2d::Vector<float>& Start() const { return _triangle.Apex(); }
    const geo2d::Vector<float>& Edge() const { return _edge; }
    int NPolygons() const { return (int)(_polygon_v.size()); }
    float LineFrac() const { return _line_frac; }
    
    const Triangle& triangle() const { return _triangle; }
    const Triangle& brem_triangle() const { return _brem_triangle; }
    const std::vector<Polygon>& Polygons() const { return _polygon_v; }
    const std::vector<Polygon>& ExpandedPolygons() const { return _expand_polygon_v; }
    const larocv::GEO2D_Contour_t& Line() const { return _line; }

    size_t Plane() const { return _plane; }

    int NBrem() const { return _n_brem; }

    float LinedX() const;
    float LinedY() const;
    
    float Charge(const larcv::Image2D& img2d, const cv::Mat& img) const;
    
    float Length() const { return _length; }

    double dQdx() const { return _dqdx; }

    int BremIndex() const { return _brem_index; }

    float Fraction(const cv::Mat& img1, const cv::Mat& img2) const;

  };

  class Object2DCollection : public std::vector<Object2D> {

  public:
    Object2DCollection() {}
    ~Object2DCollection() {}

    bool HasObject(size_t plane) const;
    const Object2D& PlaneObject(size_t plane) const;
    Object2D& PlaneObjectRW(size_t plane);
    
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

    std::vector<int> XDead(const std::array<cv::Mat,3>& dimg_v,
			   const cv::Mat& white_img,
			   float radius=3) const;
    
    
  private:
    float _theta;
    float _phi;
    float _length;
    TVector3 _start;
    float _score;

  };

}

#endif
