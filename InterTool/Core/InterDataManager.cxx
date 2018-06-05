#ifndef __INTERDATAMANAGER_CXX__
#define __INTERDATAMANAGER_CXX__

#include "InterDataManager.h"

namespace llcv { 
  
  larlite::track& InterDataManager::MakeTrack() {
    _out_track_v.resize(_out_track_v.size()+1);
    return _out_track_v.back();
  }
  
  larlite::shower& InterDataManager::MakeShower() {
    _out_shower_v.resize(_out_shower_v.size()+1);
    return _out_shower_v.back();
  }
 
 
}

#endif
