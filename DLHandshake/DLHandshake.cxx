#ifndef DLHANDSHAKE_CXX
#define DLHANDSHAKE_CXX

#include "DLHandshake.h"

namespace llcv {

  void DLHandshake::configure(const larcv::PSet&) {
    LLCV_DEBUG() << "start" << std::endl;

    LLCV_DEBUG() << "end" << std::endl;
  }

  void DLHandshake::initialize() {
    LLCV_DEBUG() << "start" << std::endl;
    _HandShaker.reset();
    LLCV_DEBUG() << "end" << std::endl;
  }

  bool DLHandshake::process(larcv::IOManager& mgr, larlite::storage_manager& sto) {
    LLCV_DEBUG() << "start" << std::endl;
    _HandShaker.reset();

    auto ev_pfpart  = (larlite::event_pfpart*)  sto.get_data(larlite::data::kPFParticle, "dl");
    auto ev_vertex  = (larlite::event_vertex*)  sto.get_data(larlite::data::kVertex,     "dl");
    auto ev_shower  = (larlite::event_shower*)  sto.get_data(larlite::data::kShower,     "dl");
    auto ev_track   = (larlite::event_track*)   sto.get_data(larlite::data::kTrack,      "dl");
    auto ev_cluster = (larlite::event_cluster*) sto.get_data(larlite::data::kCluster,    "dl");
    auto ev_hit     = (larlite::event_hit*)     sto.get_data(larlite::data::kHit,        "dl");
    auto ev_ass     = (larlite::event_ass*)     sto.get_data(larlite::data::kAssociation,"dl");
    
    if(!ev_pfpart->empty())  throw llcv_err("LL dataproduct not empty");
    if(!ev_vertex->empty())  throw llcv_err("LL dataproduct not empty");
    if(!ev_shower->empty())  throw llcv_err("LL dataproduct not empty");
    if(!ev_track->empty())   throw llcv_err("LL dataproduct not empty");
    if(!ev_cluster->empty()) throw llcv_err("LL dataproduct not empty");
    if(!ev_hit->empty())     throw llcv_err("LL dataproduct not empty");

    auto ev_hit_in  = (larlite::event_hit*)     sto.get_data(larlite::data::kHit, "gaushit");
    LLCV_DEBUG() << "GOT: " << ev_hit_in->size() << " gaushit" << std::endl;
        
    auto ev_pgraph  = (larcv::EventPGraph*) mgr.get_data(larcv::kProductPGraph,      "test_nue");
    auto ev_pixel2d = (larcv::EventPixel2D*)mgr.get_data(larcv::kProductPixel2D,"test_nue_ctor");

    LLCV_DEBUG() << "GOT: " << ev_pgraph->PGraphArray().size() << " vertices" << std::endl;
    LLCV_DEBUG() << "GOT: " << ev_pixel2d->Pixel2DArray().size() << " pixel array" << std::endl;
    LLCV_DEBUG() << "GOT: " << ev_pixel2d->Pixel2DClusterArray().size() << " pixel cluster array" << std::endl;

    _HandShaker.pixel_distance_threshold(1.);
    _HandShaker.set_larlite_pointers(ev_pfpart, ev_vertex,
				    ev_shower, ev_track,
				    ev_cluster, ev_hit,
				    ev_ass);
    
    _HandShaker.construct(*ev_pgraph, *ev_pixel2d, ev_hit_in);
    
    LLCV_DEBUG() << "end" << std::endl;
    return true;
  }
  
  void DLHandshake::finalize() {
    LLCV_DEBUG() << "start" << std::endl;
      
    LLCV_DEBUG() << "end" << std::endl;
  }
}

#endif
	