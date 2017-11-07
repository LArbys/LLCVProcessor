#ifndef TRACKTRUTHMATCH_CXX
#define TRACKTRUTHMATCH_CXX

// llcv
#include "TrackTruthMatch.h"
#include "MatchUtils.h"

// larcv
#include "DataFormat/EventImage2D.h"

// larlite
#include "DataFormat/vertex.h"
#include "DataFormat/track.h"

#include <array>

namespace llcv {

  void TrackTruthMatch::configure(const larcv::PSet&) {
    LLCV_DEBUG() << "start" << std::endl;

    LLCV_DEBUG() << "end" << std::endl;
  }

  void TrackTruthMatch::initialize() {
    LLCV_DEBUG() << "start" << std::endl;
    _tree = new TTree("TrackTruthMatch","");
    _tree->Branch("run"    , &_run    , "run/I");
    _tree->Branch("subrun" , &_subrun , "subrun/I");
    _tree->Branch("event"  , &_event  , "event/I");
    _tree->Branch("vtxid"  , &_vtxid  , "vtxid/I");
    _tree->Branch("ntracks", &_ntracks, "ntracks/I");
    _tree->Branch("npts_v" , &_npts_v);
    _tree->Branch("trk_type_v", &_trk_type_v);
    _tree->Branch("trk_type_vv", &_trk_type_vv);
    LLCV_DEBUG() << "end" << std::endl;
  }

  bool TrackTruthMatch::process(larcv::IOManager& mgr, larlite::storage_manager& sto) {
    const auto ev_vertex = (larlite::event_vertex*)sto.get_data(larlite::data::kVertex,"trackReco");
    const auto ev_seg_img = (larcv::EventImage2D*)mgr.get_data(larcv::kProductImage2D,"segment");

    LLCV_DEBUG() << "LC RSE=(" << ev_seg_img->run() << "," << ev_seg_img->subrun() << "," << ev_seg_img->event() << ") " 
		 << "& LL RSE=(" << sto.run_id()  << "," << sto.subrun_id() << "," << sto.event_id() << ")" << std::endl;

    LLCV_DEBUG() << "GOT: " << ev_vertex->size() << " vertices" << std::endl;
    
    _run    = ev_seg_img->run();
    _subrun = ev_seg_img->subrun();
    _event  = ev_seg_img->event();
    
    if (ev_vertex->empty()) return true;
    
    larlite::event_track *ev_track = nullptr;
    auto const& ass_track_vv = sto.find_one_ass(ev_vertex->id(), ev_track, ev_vertex->name());
    if (!ev_track) return true;
    
    std::array<larcv::ImageMeta,3> meta_v;
    for(size_t plane=0; plane<3; ++plane) 
      meta_v[plane] = ev_seg_img->Image2DArray()[plane].meta();
    
    double xpixel=kINVALID_DOUBLE;
    double ypixel=kINVALID_DOUBLE;

    LLCV_DEBUG() << "FOUND: " << ass_track_vv.size() << " associations" << std::endl;
    for( size_t vtx_id = 0; vtx_id < ass_track_vv.size(); ++vtx_id) {
      ClearVertex();
      const auto& vertex = ev_vertex->at(vtx_id);
      const auto& ass_track_v = ass_track_vv[vtx_id];

      _vtxid = vtx_id;
      _ntracks = (int)ass_track_v.size();
      _npts_v.resize(_ntracks);
      _trk_type_v.resize(_ntracks);
      _trk_type_vv.resize(_ntracks);

      LLCV_DEBUG() << "@vtx_id=" << vtx_id << std::endl;
      LLCV_DEBUG() << "..." << ass_track_vv[vtx_id].size() << " associated tracks" << std::endl;

      for(size_t aid=0; aid<ass_track_v.size(); ++aid) {
	auto assid = ass_track_v[aid];
	const auto& track = ev_track->at(assid);

	_npts_v[aid] = track.NumberTrajectoryPoints();
	auto& pt_type_v = _trk_type_vv[aid];
	pt_type_v.clear();
	pt_type_v.resize(larcv::kROITypeMax+1,0);

	LLCV_DEBUG() << "@track=" << aid << " sz=" << track.NumberTrajectoryPoints() << std::endl;

	for(size_t pid=0; pid< track.NumberTrajectoryPoints(); ++pid) {
	  const auto& pt = track.LocationAtPoint(pid);

	  LLCV_DEBUG() << "@pid: "<<pid<<"=(" << pt.X() << ","  << pt.Y() << "," << pt.Z() << ")" << std::endl;

	  for(size_t plane=0; plane<3; ++plane) {
	    const auto& plane_img = ev_seg_img->Image2DArray()[plane];
	    xpixel = kINVALID_DOUBLE;
	    ypixel = kINVALID_DOUBLE;
	    Project3D(meta_v[plane],vertex.X(),vertex.Y(),vertex.Z(),0.0,plane,xpixel,ypixel);
	    int xx = (int)(xpixel+0.5);
	    int yy = (int)(ypixel+0.5);
	    yy = plane_img.meta().rows() - yy - 1;
	    float pixel_type = plane_img.pixel(yy,xx);
	    pt_type_v.at((size_t)pixel_type) += 1;

	    LLCV_DEBUG() << "p:" << plane << "(" << yy << "," << xx << ")=" << (size_t)pixel_type << std::endl;

	  } // end plane
	} // end 3D point

	auto res_iter = std::max_element(std::begin(pt_type_v), std::end(pt_type_v));
	auto res_loc  = std::distance(std::begin(pt_type_v), res_iter);

	_trk_type_v[aid] = (int)res_loc;

      } // end track

      _tree->Fill();
    } // end vertex
    
    LLCV_DEBUG() << "end" << std::endl;
    return true;
  }
  
  void TrackTruthMatch::finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    _tree->Write();
    LLCV_DEBUG() << "end" << std::endl;
  }

  void TrackTruthMatch::ClearVertex() {
    _vtxid   = larcv::kINVALID_INT;
    _ntracks = larcv::kINVALID_INT;
    _npts_v.clear();
    _trk_type_v.clear();
    _trk_type_vv.clear();
  }

}

#endif
	
