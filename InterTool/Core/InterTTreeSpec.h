#ifndef __INTERTTREESPEC_H__
#define __INTERTTREESPEC_H__

#include "LLCVBase/llcv_err.h"
#include "TChain.h"
#include <map>

namespace llcv {

  class InterTTreeSpec {
  public:
    void LoadTree(TChain& tc);

    std::map<std::string, int*> _imap;
    std::map<std::string, double*> _dmap;
    std::map<std::string, float*> _fmap;
    std::map<std::string, std::vector<float>* > _vmap;
    std::map<std::string, std::vector<std::vector<float> >* > _vvmap;

    int* run;
    int* subrun;
    int* event;
    double* vtxid;

  };

}


#endif 
