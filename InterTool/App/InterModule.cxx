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

    _track_vertex_prod  = cfg.get<std::string>("TrackVertexProducer");
    _shower_vertex_prod = cfg.get<std::string>("ShowerVertexProducer");
    _opflash_prod       = cfg.get<std::string>("OpFlashProducer");

    LLCV_DEBUG() << "adc_img_prod........." << _adc_img_prod << std::endl;
    LLCV_DEBUG() << "trk_img_prod........." << _trk_img_prod << std::endl;
    LLCV_DEBUG() << "shr_img_prod........." << _shr_img_prod << std::endl;
    LLCV_DEBUG() << "pgraph_prod.........." << _pgraph_prod << std::endl;
    LLCV_DEBUG() << "pixel_prod..........." << _pixel_prod << std::endl;
    LLCV_DEBUG() << "track_vertex_prod... " << _track_vertex_prod << std::endl;
    LLCV_DEBUG() << "shower_vertex_prod.. " << _shower_vertex_prod << std::endl;
    LLCV_DEBUG() << "opflash_prod........." << _opflash_prod << std::endl;
    
    LLCV_DEBUG() << "end" << std::endl;
  }

  void InterModule::initialize() {
    LLCV_DEBUG() << "start" << std::endl;
    LLCV_DEBUG() << "end" << std::endl;
  }

  bool InterModule::process(larcv::IOManager& mgr, larlite::storage_manager& sto) {
    LLCV_DEBUG() << "start" << std::endl;
    LLCV_DEBUG() << "@sto (r,s,e,e)=(" 
		 << sto.run_id()    << "," 
		 << sto.subrun_id() << "," 
		 << sto.event_id()  << "," 
		 << sto.get_index() << ")" <<std::endl;

    //
    // larcv data products
    //
    larcv::EventImage2D* ev_adc_img = nullptr;
    if (!_adc_img_prod.empty()) 
      ev_adc_img = (larcv::EventImage2D*)mgr.get_data(larcv::kProductImage2D,_adc_img_prod);
    else 
      throw llcv_err("Must specify ADC image");
    
    LLCV_DEBUG() << "@mgr (r,s,e,e)=(" 
		 << ev_adc_img->run()    << "," 
		 << ev_adc_img->subrun() << "," 
		 << ev_adc_img->event()  << "," 
		 << mgr.current_entry()  << ")" << std::endl;

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
    larlite::event_vertex* ev_shower_vertex = nullptr;
    if(!_shower_vertex_prod.empty())
      ev_shower_vertex = (larlite::event_vertex*)sto.get_data(larlite::data::kVertex,_shower_vertex_prod);

    larlite::event_vertex* ev_track_vertex = nullptr;
    if(!_track_vertex_prod.empty())
      ev_track_vertex = (larlite::event_vertex*)sto.get_data(larlite::data::kVertex,_track_vertex_prod);

    larlite::event_opflash* ev_opflash = nullptr;
    if(!_opflash_prod.empty())
      ev_opflash = (larlite::event_opflash*)sto.get_data(larlite::data::kOpFlash,_opflash_prod);
    
    
    std::cout << "tvtx=" << ev_track_vertex->size() << " svtx=" << ev_shower_vertex->size() << std::endl;

    assert (ev_track_vertex->size() == ev_shower_vertex->size());
      
    if (ev_track_vertex->empty()) {
      // Fill something?
      LLCV_DEBUG() << "NO VERTEX" << std::endl;
      return true;
    }
    
    LLCV_DEBUG() << "GOT: " << ev_track_vertex->size() << " VERTEX" << std::endl;

    LLCV_DEBUG() << "end" << std::endl;
    return true;
  }
  
  void InterModule::finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    LLCV_DEBUG() << "end" << std::endl;
  }

}

#endif
	
