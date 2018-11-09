#ifndef PLAYAROUND_CXX
#define PLAYAROUND_CXX

// llcv
#include "PlayAround.h"

// larcv
#include "DataFormat/EventImage2D.h"

// larlite
#include "DataFormat/hit.h"

#include <array>
#include <cassert>

namespace llcv {

  void PlayAround::configure(const larcv::PSet& cfg) {
    LLCV_DEBUG() << "start" << std::endl;

    LLCV_DEBUG() << "end" << std::endl;
  }

  void PlayAround::initialize() {
    LLCV_DEBUG() << "start" << std::endl;

    _tree = new TTree("PlayAround","");
    _tree->Branch("run"     , &_run     , "run/I");
    _tree->Branch("subrun"  , &_subrun  , "subrun/I");
    _tree->Branch("event"   , &_event   , "event/I");

    LLCV_DEBUG() << "end" << std::endl;
  }

  bool PlayAround::process(larcv::IOManager& mgr, larlite::storage_manager& sto) {
    LLCV_DEBUG() << "start" << std::endl;

    
 
    LLCV_DEBUG() << "end" << std::endl;
    return true;
  }
  
  void PlayAround::finalize() {
    LLCV_DEBUG() << "start" << std::endl;

    _tree->Write();

    LLCV_DEBUG() << "end" << std::endl;
  }

}

#endif
	
