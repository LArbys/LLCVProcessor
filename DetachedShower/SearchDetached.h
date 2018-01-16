#ifndef SEARCHDETACHED_H
#define SEARCHDETACHED_H

// me
#include "SearchAlgoBase.h"

//llcv
#include "LLCVBase/AnaBase.h"

//lcv
#include "LArbysImageMaker.h"

//ll

namespace llcv {
  
  class SearchDetached : public AnaBase {
    
  public:
    
  SearchDetached(const std::string name="SearchDetached") : AnaBase(name), _algo(nullptr) {}
    ~SearchDetached() {}
    
    void configure(const larcv::PSet& cfg);
    void initialize();
    bool process(larcv::IOManager& mgr, larlite::storage_manager& sto);
    void finalize();

    const larcv::LArbysImageMaker& LArMkr() const { return _larmkr; }

    void MaskImage(const std::vector<larcv::PGraph>& pgraph_v,
		   const std::map<larcv::PlaneID_t, std::vector<larcv::Pixel2DCluster> >& pix_m,
		   const std::map<larcv::PlaneID_t, std::vector<larcv::ImageMeta> >&   pix_meta_m,
		   std::vector<larcv::Image2D>& img_v);

    void SetAlgo(SearchAlgoBase* algo) 
    { if(!algo) throw llcv_err("Invalid algo specified"); _algo = algo; }

  private:

    SearchAlgoBase* _algo;
    
    std::string _adc_img_prod;
    std::string _shr_img_prod;
    std::string _pgraph_prod;
    std::string _pixel_prod;

    std::string _out_pgraph_prod;
    std::string _out_pixel_prod;
    
    float _search_distance;

    larcv::LArbysImageMaker _larmkr;

  };

}

#endif
