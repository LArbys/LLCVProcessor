#ifndef __INTERTTREEMANAGER_CXX__
#define __INTERTTREEMANAGER_CXX__

#include "InterTTreeManager.h"

namespace llcv {
  
  void InterTTreeManager::Initialize(const std::string& fname,
				     const std::string& tname) {
    _tchain = new TChain(tname.c_str());
    _tchain->Add(fname.c_str());
    _nentries = (size_t)_tchain->GetEntries();
    _spec.LoadTree(*_tchain);
  }

  bool InterTTreeManager::Next() {
    if (_centry==kINVALID_SIZE) {
      _centry = 0;
      _tchain->GetEntry(_centry);
      return true;
    }

    if (_centry >= _nentries) {
      LLCV_WARNING() << "Called next too many times..." << std::endl;
      return false;
    }
    
    _centry++;

    _tchain->GetEntry(_centry);

    return true;
  }



}


#endif

//hi mom
