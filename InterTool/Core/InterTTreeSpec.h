#ifndef __INTERTTREESPEC_H__
#define __INTERTTREESPEC_H__

#include "LLCVBase/llcv_err.h"
#include "TChain.h"
#include <unordered_map>

namespace llcv {

  class InterTTreeSpec {
  public:
    void LoadTree(TChain& tc);

    std::unordered_map<std::string, int> _imap;
    std::unordered_map<std::string, double> _dmap;
    std::unordered_map<std::string, float> _fmap;
    std::unordered_map<std::string, std::vector<float>* > _vmap;
    std::unordered_map<std::string, std::vector<std::vector<float> >* > _vvmap;

    int* run;
    int* subrun;
    int* event;
    double* vtxid;

  };

}


#endif 
