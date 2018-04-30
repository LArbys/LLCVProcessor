#ifndef __SELCLUSTER3D_H__
#define __SELCLUSTER3D_H__

#include "InterTool_Core/InterSelBase.h"

#include "LArOpenCV/ImageCluster/AlgoClass/PixelScan3D.h"

#include "TStopwatch.h"

namespace llcv {
  

  class SelCluster3D : public InterSelBase { 

  public:

  SelCluster3D(std::string name="SelCluster3D") : InterSelBase(name), _outtree(nullptr) {}
    ~SelCluster3D(){}
    
    void Configure (const larcv::PSet &pset);
    void Initialize();
    double Select();
    void Finalize();
    
  private:
    TTree* _outtree;

    //
    // Members
    //
  private:

    size_t _cropx;
    size_t _cropy;

    larocv::PixelScan3D _PixelScan3D;

    TStopwatch _twatch;

    void ResizeOutput(size_t sz);

    std::pair<float,float> ShowerAngle(const larlite::shower& shower);

    //
    // TTree
    //
  private:

    std::vector<float> _shower_x_v;
    std::vector<float> _shower_y_v;
    std::vector<float> _shower_z_v;

  };

}


#endif
