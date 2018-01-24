#ifndef __INTERMODULE_CXX__
#define __INTERMODULE_CXX__

#include "InterModule.h"

// lcv
#include "DataFormat/Vertex.h"
#include "DataFormat/EventPGraph.h"
#include "DataFormat/EventPixel2D.h"

// ll
#include "DataFormat/vertex.h"
#include "DataFormat/pfpart.h"
#include "DataFormat/track.h"
#include "DataFormat/shower.h"
#include "DataFormat/cluster.h"
#include "DataFormat/hit.h"
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
    _shower_shower_prod = cfg.get<std::string>("ShowerShowerProducer");
    _opflash_prod       = cfg.get<std::string>("OpFlashProducer");

    LLCV_DEBUG() << "adc_img_prod........." << _adc_img_prod << std::endl;
    LLCV_DEBUG() << "trk_img_prod........." << _trk_img_prod << std::endl;
    LLCV_DEBUG() << "shr_img_prod........." << _shr_img_prod << std::endl;
    LLCV_DEBUG() << "pgraph_prod.........." << _pgraph_prod << std::endl;
    LLCV_DEBUG() << "pixel_prod..........." << _pixel_prod << std::endl;
    LLCV_DEBUG() << "track_vertex_prod... " << _track_vertex_prod << std::endl;
    LLCV_DEBUG() << "shower_vertex_prod.. " << _shower_vertex_prod << std::endl;
    LLCV_DEBUG() << "opflash_prod........." << _opflash_prod << std::endl;
    
    _epsilon = cfg.get<float>("EPS",1e-5);

    _driver.Configure(cfg.get<larcv::PSet>(_driver.name()));

    for(auto selptr : _driver._sel_base_v)
      selptr->Configure(cfg.get<larcv::PSet>(selptr->name()));

    LLCV_DEBUG() << "end" << std::endl;
  }

  void InterModule::initialize() {
    LLCV_DEBUG() << "start" << std::endl;
    _driver.Initialize();
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

    _driver._run    = (size_t)sto.run_id();
    _driver._subrun = (size_t)sto.subrun_id();
    _driver._event  = (size_t)sto.event_id();

    _driver.AttachImage(ev_adc_img->Image2DArray(),kImageADC);

    larcv::EventImage2D* ev_trk_img = nullptr;
    if(!_trk_img_prod.empty()){
      ev_trk_img = (larcv::EventImage2D*)mgr.get_data(larcv::kProductImage2D,_trk_img_prod);
      _driver.AttachImage(ev_trk_img->Image2DArray(),kImageTrack);
    }
    
    larcv::EventImage2D* ev_shr_img = nullptr;
    if(!_shr_img_prod.empty()) {
      ev_shr_img = (larcv::EventImage2D*)mgr.get_data(larcv::kProductImage2D,_shr_img_prod);
      _driver.AttachImage(ev_shr_img->Image2DArray(),kImageShower);
    }
    
    larcv::EventPGraph* ev_pgraph = nullptr;
    if(!_pgraph_prod.empty())
      ev_pgraph = (larcv::EventPGraph*) mgr.get_data(larcv::kProductPGraph ,_pgraph_prod);

    larcv::EventPixel2D* ev_pixel = nullptr;
    if(!_pixel_prod.empty())
      ev_pixel = (larcv::EventPixel2D*)mgr.get_data(larcv::kProductPixel2D,_pixel_prod);
    
    //
    // larlite data products
    //
    larlite::event_vertex* ev_track_vertex = nullptr;
    if(!_track_vertex_prod.empty())
      ev_track_vertex = (larlite::event_vertex*)sto.get_data(larlite::data::kVertex,_track_vertex_prod);


    larlite::event_vertex* ev_shower_vertex = nullptr;
    if(!_shower_vertex_prod.empty())
      ev_shower_vertex = (larlite::event_vertex*)sto.get_data(larlite::data::kVertex,_shower_vertex_prod);


    larlite::event_opflash* ev_opflash = nullptr;
    if(!_opflash_prod.empty())
      ev_opflash = (larlite::event_opflash*)sto.get_data(larlite::data::kOpFlash,_opflash_prod);
    
    
    //
    // configure the driver
    //
    std::cout << "tvtx=" << ev_track_vertex->size() << " svtx=" << ev_shower_vertex->size() << std::endl;
    
    auto ntrack_vertex = ev_track_vertex->size();
    auto nshower_vertex= ev_shower_vertex->size();
    auto npgraph_vertex= ev_pgraph->PGraphArray().size();
    
    assert(ntrack_vertex == nshower_vertex);
    assert(ntrack_vertex == npgraph_vertex);
    
    if (ev_track_vertex->empty()) {
      // Fill something?
      LLCV_DEBUG() << "NO VERTEX" << std::endl;
      return true;
    }
    
    LLCV_DEBUG() << "GOT: " << ev_track_vertex->size() << " VERTEX" << std::endl;

    // get tracks
    larlite::event_track *ev_track = nullptr;
    auto const& ass_track_vv = sto.find_one_ass(ev_track_vertex->id(), ev_track, ev_track_vertex->name());

    // get showers, cluster, hits
    larlite::event_pfpart *ev_pfpart = nullptr;
    larlite::event_shower *ev_shower = nullptr;
    larlite::event_cluster *ev_cluster = nullptr;
    larlite::event_hit *ev_hit = nullptr;

    std::vector<std::vector<unsigned int> > ass_shower_vv;
    std::vector<std::vector<unsigned int> > ass_cluster_vv;
    std::vector<std::vector<unsigned int> > ass_hit_vv;

    auto ass_pfpart_vv = sto.find_one_ass(ev_shower_vertex->id(), ev_pfpart, ev_shower_vertex->name());
    
    if (ev_pfpart)
      ass_shower_vv = sto.find_one_ass(ev_pfpart->id(), ev_shower, _shower_shower_prod);

    if (ev_shower)
      ass_cluster_vv = sto.find_one_ass(ev_shower->id(), ev_cluster, ev_shower->name());

    if (ev_cluster)
      ass_hit_vv = sto.find_one_ass(ev_cluster->id(), ev_hit, ev_cluster->name());

    
    for(size_t vtxid=0; vtxid < ntrack_vertex; ++vtxid) {
      
      LLCV_DEBUG() << "@vtxid=" << vtxid << std::endl;

      const auto& track_vertex  = (*ev_track_vertex)[vtxid];
      const auto& shower_vertex = (*ev_shower_vertex)[vtxid];
      const auto& pgraph_vertex = ev_pgraph->PGraphArray()[vtxid];
      
      AssertEqual(track_vertex,shower_vertex);
      AssertEqual(track_vertex,pgraph_vertex);
      
      // attach vertex & pgraph
      auto vid  = _driver.AttachVertex(&track_vertex);
      auto pgid = _driver.AttachPGraph(vid,&pgraph_vertex);
      
      // attach tracks
      const auto& ass_track_v = ass_track_vv[vtxid];
      for(size_t trk_id=0; trk_id<ass_track_v.size(); ++trk_id) {
	auto track_id = ass_track_v[trk_id];
	auto& track = ev_track->at(track_id);
	auto tid = _driver.AttachTrack(vid,&track);
      }
      
      // attach shower cluster hit
      const auto& ass_pfpart_v = ass_pfpart_vv[vtxid];
      for( size_t pfp_id=0; pfp_id < ass_pfpart_v.size(); ++ pfp_id) {
	const auto pfpart_id = ass_pfpart_v.at(pfp_id);

	// edge case
	if (pfpart_id >= ass_shower_vv.size()) continue;

	const auto& ass_shower_v = ass_shower_vv.at(pfpart_id);
	for(size_t shr_id=0; shr_id < ass_shower_v.size(); ++ shr_id) {
	  const auto shower_id = ass_shower_v.at(shr_id);
	  const auto& shower = ev_shower->at(shower_id);

	  // attach shower
	  auto sid = _driver.AttachShower(vid,&shower);

	  const auto& ass_cluster_v = ass_cluster_vv.at(shower_id);
	  for(size_t clu_id=0; clu_id < ass_cluster_v.size(); ++ clu_id) {
	    const auto cluster_id = ass_cluster_v.at(clu_id);
	    const auto& cluster= ev_cluster->at(cluster_id);

	    // attach cluster
	    auto cid = _driver.AttachCluster(vid,sid,&cluster);
	    
	    const auto& ass_hit_v = ass_hit_vv.at(shower_id);
	    for(size_t h_id=0; h_id < ass_hit_v.size(); ++ h_id) {
	      const auto hit_id = ass_hit_v.at(h_id);
	      const auto& hit = ev_hit->at(hit_id);

	      // attach hit
	      auto hid = _driver.AttachHit(vid,sid,cid,&hit);
	    }
	  }
	}
      }

    } // end vertex

    _driver.Dump();
    _driver.Process();
    _driver.Reset();

    LLCV_DEBUG() << "end" << std::endl;
    return true;
  }

  void InterModule::finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    LLCV_DEBUG() << "end" << std::endl;
  }

  void InterModule::AssertEqual(const larlite::vertex& vtx1,
				const larlite::vertex& vtx2) {

    LLCV_DEBUG() << "Compare vtx1=(" << vtx1.X() << "," << vtx1.Y() << "," << vtx1.Z() << ")" << std::endl;
    LLCV_DEBUG() << "Compare vtx2=(" << vtx2.X() << "," << vtx2.Y() << "," << vtx2.Z() << ")" << std::endl;

    if ((std::abs(vtx1.X() - vtx2.X()) > _epsilon) or
	(std::abs(vtx1.Y() - vtx2.Y()) > _epsilon) or
	(std::abs(vtx1.Z() - vtx2.Z()) > _epsilon))
      throw llcv_err("Vertex 1 & 2 are not equivalent");

    return;
  }

  void InterModule::AssertEqual(const larlite::vertex& vtx1,
				const larcv::PGraph& pgraph) {
    
    const auto& vtx2 = pgraph.ParticleArray().front();

    LLCV_DEBUG() << "Compare vtx1=(" << vtx1.X() << "," << vtx1.Y() << "," << vtx1.Z() << ")" << std::endl;
    LLCV_DEBUG() << "Compare vtx2=(" << vtx2.X() << "," << vtx2.Y() << "," << vtx2.Z() << ")" << std::endl;

    if ((std::abs(vtx1.X() - vtx2.X()) > _epsilon) or
	(std::abs(vtx1.Y() - vtx2.Y()) > _epsilon) or
	(std::abs(vtx1.Z() - vtx2.Z()) > _epsilon))
      throw llcv_err("Vertex 1 & 2 are not equivalent");

    return;
  }

}

#endif
	
