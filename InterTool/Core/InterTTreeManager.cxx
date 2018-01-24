#ifndef __INTERTTREEMANAGER_CXX__
#define __INTERTTREEMANAGER_CXX__

#include "InterTTreeManager.h"
#include "InterTTreeManager.imp.h"

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

  template int    InterTTreeManager::Scalar<int   >(const std::string& str) const;
  template float  InterTTreeManager::Scalar<float >(const std::string& str) const;
  template double InterTTreeManager::Scalar<double>(const std::string& str) const;
  
  
  std::vector<float> InterTTreeManager::Vector(const std::string& str) const {
    
    static std::unordered_map<std::string, std::vector<float>*>::const_iterator search_v;
    
    search_v = _spec._vmap.find(str);
    if(search_v != _spec._vmap.end())
      return *(search_v->second);
    
    LLCV_CRITICAL() << "Could not find vector " << str << " in TTree" << std::endl;
    throw llcv_err("die");
    
    static std::vector<float> res;      
    return res;
    
  }
  
  std::vector<std::vector<float> > InterTTreeManager::VVector(const std::string& str) const {

    static std::unordered_map<std::string, std::vector<std::vector<float> >*>::const_iterator search_vv;
    
    search_vv = _spec._vvmap.find(str);
    if(search_vv != _spec._vvmap.end())
      return *(search_vv->second);
    
    LLCV_CRITICAL() << "Could not find vector vector " << str << " in TTree" << std::endl;
    throw llcv_err("die");
    
    static std::vector<std::vector<float> > res;      
    return res;
    
  }


}


#endif

//hi mom
