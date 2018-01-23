#ifndef __INTERMODULE_CXX__
#define __INTERMODULE_CXX__

#include "InterModule.h"

// lcv
#include "DataFormat/Vertex.h"
#include "DataFormat/EventPGraph.h"
#include "DataFormat/EventPixel2D.h"

// ll
#include "DataFormat/vertex.h"
#include "DataFormat/track.h"
#include "DataFormat/shower.h"
#include "DataFormat/opflash.h"


namespace llcv {

  void InterModule::configure(const larcv::PSet& cfg) {
    LLCV_DEBUG() << "start" << std::endl;
    
    _adc_img_prod = cfg.get<std::string>("ADCImageProducer");
    _trk_img_prod = cfg.get<std::string>("TrackImageProducer");
    _shr_img_prod = cfg.get<std::string>("ShowerImageProducer");
    
    _pgraph_prod = cfg.get<std::string>("PGraphProducer");
    _pixel_prod  = cfg.get<std::string>("PixelProducer");

    _vertex_prod  = cfg.get<std::string>("VertexProducer");
    _shower_prod  = cfg.get<std::string>("ShowerProducer");
    _track_prod   = cfg.get<std::string>("TrackProducer");
    _opflash_prod = cfg.get<std::string>("OpFlashProducer");
    
    LLCV_DEBUG() << "end" << std::endl;
  }

  void InterModule::initialize() {
    LLCV_DEBUG() << "start" << std::endl;
    LLCV_DEBUG() << "end" << std::endl;
  }

  bool InterModule::process(larcv::IOManager& mgr, larlite::storage_manager& sto) {
    LLCV_DEBUG() << "start" << std::endl;

    //
    // larcv data products
    //
    larcv::EventImage2D* ev_adc_img = nullptr;
    if (!_adc_img_prod.empty())
      ev_adc_img = (larcv::EventImage2D*)mgr.get_data(larcv::kProductImage2D,_adc_img_prod);
    
    larcv::EventImage2D* ev_trk_img = nullptr;
    if(!_trk_img_prod.empty())
      ev_trk_img = (larcv::EventImage2D*)mgr.get_data(larcv::kProductImage2D,_trk_img_prod);
    
    larcv::EventImage2D* ev_shr_img = nullptr;
    if(!_shr_img_prod.empty())
      ev_shr_img = (larcv::EventImage2D*)mgr.get_data(larcv::kProductImage2D,_shr_img_prod);
    
    larcv::EventPGraph* ev_pgraph = nullptr;
    if(!_pgraph_prod.empty())
      ev_pgraph = (larcv::EventPGraph*) mgr.get_data(larcv::kProductPGraph ,_pgraph_prod);

    larcv::EventPixel2D* ev_pixel = nullptr;
    if(!_pixel_prod.empty())
      ev_pixel = (larcv::EventPixel2D*)mgr.get_data(larcv::kProductPixel2D,_pixel_prod);
    
    //
    // larlite data products
    //
    larlite::event_vertex* ev_vertex = nullptr;
    if(!_vertex_prod.empty())
       ev_vertex = (larlite::event_vertex*)sto.get_data(larlite::data::kVertex,_vertex_prod);

    larlite::event_track* ev_track = nullptr;
    if(!_track_prod.empty())
       ev_track = (larlite::event_track*)sto.get_data(larlite::data::kTrack,_track_prod);

    larlite::event_shower* ev_shower = nullptr;
    if(!_shower_prod.empty())
       ev_shower = (larlite::event_shower*)sto.get_data(larlite::data::kShower,_shower_prod);

    larlite::event_opflash* ev_opflash = nullptr;
    if(!_opflash_prod.empty())
       ev_opflash = (larlite::event_opflash*)sto.get_data(larlite::data::kOpFlash,_opflash_prod);
    
    LLCV_DEBUG() << "end" << std::endl;
    return true;
  }
  
  void InterModule::finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    LLCV_DEBUG() << "end" << std::endl;
  }

}

#endif
	
