#ifndef __SELNUEID_H__
#define __SELNUEID_H__

#include "InterTool_Core/InterSelBase.h"
#include "TStopwatch.h"
#include <array>
#include "ContourScan.h"

namespace llcv {
  
  class SelNueID : public InterSelBase { 

  public:

  SelNueID(std::string name="SelNueID") : InterSelBase(name), _outtree(nullptr) {}
    ~SelNueID(){}
      
    void Configure (const larcv::PSet &pset);
    void Initialize();
    double Select();
    void Finalize();


  private:
    TTree* _outtree;
    size_t _cropx;
    size_t _cropy;
    size_t _n_neighbors;
    

    ContourScan _ContourScan;

    TStopwatch _twatch;

  private:
    size_t FindClosestContour(const larocv::GEO2D_ContourArray_t& ctor_v,
			      const geo2d::Vector<float>& pt);

  private:

    float _vertex_x;
    float _vertex_y;
    float _vertex_z;
    
    std::vector<int> _nctor_v;
    
    std::vector<std::vector<float> > _line_frac_vv;
    std::vector<std::vector<float> > _line_len_vv;
    
  };

}


#endif
