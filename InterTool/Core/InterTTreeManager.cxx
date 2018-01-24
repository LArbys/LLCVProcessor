#ifndef __INTERTTREEMANAGER_CXX__
#define __INTERTTREEMANAGER_CXX__

#include "InterTTreeManager.h"

namespace llcv {
  
  void InterTTreeManager::Configure(const larcv::PSet& cfg) {
    set_verbosity((msg::Level_t)cfg.get<int>("Verbosity",2));
    LLCV_DEBUG() << "start" << std::endl;

    LLCV_DEBUG() << "end" << std::endl;
  }

  void InterTTreeManager::Initialize(const std::string& fname,
				     const std::string& tname) {
    LLCV_DEBUG() << "tname=" << tname << std::endl;
    LLCV_DEBUG() << "fname=" << fname << std::endl;
    _tchain = new TChain(tname.c_str());
    _tchain->Add(fname.c_str());
    _nentries = (size_t)_tchain->GetEntries();
    _spec.LoadTree(*_tchain);
    Next();
  }

  bool InterTTreeManager::Next() {
    LLCV_DEBUG() << "now centry=" << _centry << std::endl;
    if (_centry==kINVALID_SIZE) {
      _centry = 0;
      _tchain->GetEntry(_centry);
      LLCV_DEBUG() << "now centry=" << _centry << std::endl;
      return true;
    }

    if (_centry >= _nentries) {
      LLCV_WARNING() << "Called next too many times..." << std::endl;
      return false;
    }
    
    _centry++;

    _tchain->GetEntry(_centry);
    LLCV_DEBUG() << "now centry=" << _centry << std::endl;
    return true;
  }

}


#endif

//hi mom
