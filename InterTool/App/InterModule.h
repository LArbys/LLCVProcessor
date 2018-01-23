#ifndef __INTERMODULE_H__
#define __INTERMODULE_H__

//llcv
#include "LLCVBase/AnaBase.h"

#include "InterTool_Core/InterDriver.h"

namespace llcv {
  
  class InterModule : public AnaBase {
    
  public:
    
  InterModule(const std::string name="InterModule") :  AnaBase(name) {}
    ~InterModule() {}
    
    void configure(const larcv::PSet& cfg);
    void initialize();
    bool process(larcv::IOManager& mgr, larlite::storage_manager& sto);
    void finalize();

    
  private:
    
    std::string _adc_img_prod;
    std::string _trk_img_prod;
    std::string _shr_img_prod;
    
    std::string _pgraph_prod;
    std::string _pixel_prod;
    
    std::string _vertex_prod;
    std::string _shower_prod;
    std::string _track_prod;
    std::string _opflash_prod;
    
  };

}

#endif
