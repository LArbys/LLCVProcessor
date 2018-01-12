#ifndef SEARCHDETACHED_H
#define SEARCHDETACHED_H

//llcv
#include "LLCVBase/AnaBase.h"

//lcv
#include "LArbysImageMaker.h"

//ll

namespace llcv {
  
  class SearchDetached : public AnaBase {
    
  public:
    
  SearchDetached(const std::string name="SearchDetached") : AnaBase(name) {}
    ~SearchDetached() {}

    void configure(const larcv::PSet& cfg);
    void initialize();
    bool process(larcv::IOManager& mgr, larlite::storage_manager& sto);
    void finalize();

    const larcv::LArbysImageMaker& LArMkr() const { return _larmkr; }
    
  private:
    std::string _adc_img_prod;
    std::string _shr_img_prod;
    std::string _pgraph_prod;
    std::string _pixel_prod;
    
    larcv::LArbysImageMaker _larmkr;

  };

}

#endif
