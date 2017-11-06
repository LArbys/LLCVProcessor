#ifndef TRACKTRUTHMATCH_CXX
#define TRACKTRUTHMATCH_CXX

#include "TrackTruthMatch.h"
#include "DataFormat/track.h"

namespace llcv {

  void TrackTruthMatch::configure(const larcv::PSet&) {
    LLCV_DEBUG() << "start" << std::endl;

    LLCV_DEBUG() << "end" << std::endl;
  }

  void TrackTruthMatch::initialize() {
    LLCV_DEBUG() << "start" << std::endl;

    
    
    LLCV_DEBUG() << "end" << std::endl;
  }

  bool TrackTruthMatch::process(larcv::IOManager& mgr, larlite::storage_manager& sto) {
    LLCV_DEBUG() << "start" << std::endl;
    const auto ev_track = (larlite::event_track*)sto.get_data(larlite::data::kTrack,"trackReco");
    
    std::cout << "see... " << ev_track->size() << std::endl;
    
    
    LLCV_DEBUG() << "end" << std::endl;
    return true;
  }
  
  void TrackTruthMatch::finalize() {
    LLCV_DEBUG() << "start" << std::endl;
      
    LLCV_DEBUG() << "end" << std::endl;
  }
}

#endif
	
