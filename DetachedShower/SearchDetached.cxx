#ifndef SEARCHDETACHED_CXX
#define SEARCHDETACHED_CXX

// llcv
#include "SearchDetached.h"

// larcv
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventPGraph.h"
#include "DataFormat/EventPixel2D.h"

// larlite
#include "DataFormat/vertex.h"
#include "DataFormat/pfpart.h"
#include "DataFormat/shower.h"

#include <array>
#include <cassert>

namespace llcv {

  void SearchDetached::configure(const larcv::PSet& cfg) {
    LLCV_DEBUG() << "start" << std::endl;

    _adc_img_prod = cfg.get<std::string>("ADCImageProducer");
    _shr_img_prod = cfg.get<std::string>("ShowerImageProducer");
    _pgraph_prod  = cfg.get<std::string>("PGraphProducer");
    _pixel_prod   = cfg.get<std::string>("Pixel2DProducer");

    _larmkr.Configure(cfg.get<larcv::PSet>("LArbysImageMaker"));

    LLCV_DEBUG() << "end" << std::endl;
  }

  void SearchDetached::initialize() {
    LLCV_DEBUG() << "start" << std::endl;


    LLCV_DEBUG() << "end" << std::endl;
  }

  bool SearchDetached::process(larcv::IOManager& mgr, larlite::storage_manager& sto) {
    LLCV_DEBUG() << "start" << std::endl;

    auto ev_adc_img = (larcv::EventImage2D*)mgr.get_data(larcv::kProductImage2D,_adc_img_prod);
    auto ev_shr_img = (larcv::EventImage2D*)mgr.get_data(larcv::kProductImage2D,_shr_img_prod);
    
    auto ev_pgraph  = (larcv::EventPGraph*) mgr.get_data(larcv::kProductPGraph ,_pgraph_prod);
    auto ev_pixel   = (larcv::EventPixel2D*)mgr.get_data(larcv::kProductPixel2D,_pixel_prod);
    
    //
    // get the shower image
    //
    const auto& shr_img_v = ev_shr_img->Image2DArray();
    const auto& adc_img_v = ev_adc_img->Image2DArray();
    
    auto mat_v = _larmkr.ExtractMat(shr_img_v);
    cv::imwrite("shower.png",mat_v.front());

    mat_v = _larmkr.ExtractMat(adc_img_v);
    cv::imwrite("adc.png",mat_v.front());

    LLCV_DEBUG() << "end" << std::endl;
    return true;
  }
  
  void SearchDetached::finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    
    LLCV_DEBUG() << "end" << std::endl;
  }

}

#endif
	
