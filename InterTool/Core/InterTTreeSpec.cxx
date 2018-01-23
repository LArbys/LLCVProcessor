#ifndef __INTERTTREESPEC_CXX__
#define __INTERTTREESPEC_CXX__

#include "InterTTreeSpec.h"
#include "InterToolTypes.h"

#include "TLeafElement.h"

#include <iostream>
namespace llcv {

  void InterTTreeSpec::LoadTree(TChain& tc) {
    auto leaves = tc.GetListOfLeaves();

    for(size_t lid=0; lid<(size_t)leaves->GetEntries(); ++lid) {
      auto leaf = (TLeafElement*)leaves->At(lid);
      auto name  = std::string(leaf->GetName());
      auto type  = std::string(leaf->GetTypeName());
      auto stype = LeafToSpecType(type);
      switch (stype) {
      case kINT  :   tc.SetBranchAddress(name.c_str(),&_imap[name]);  break;
      case kFLOAT:   tc.SetBranchAddress(name.c_str(),&_fmap[name]);  break;
      case kDOUBLE:  tc.SetBranchAddress(name.c_str(),&_dmap[name]);  break;
      case kVFLOAT:  tc.SetBranchAddress(name.c_str(),&_vmap[name]);  break;
      case kVVFLOAT: tc.SetBranchAddress(name.c_str(),&_vvmap[name]); break;
      default: 
	std::cout << "@name=" << name << " @type=" << type << " is unknown" << std::endl;
	throw llcv_err("Unknown type");
      };
    }
    
    run    = _imap.at("run");
    subrun = _imap.at("subrun");
    event  = _imap.at("event");
    vtxid  = _dmap.at("vtxid");
    
  }

}


#endif 
