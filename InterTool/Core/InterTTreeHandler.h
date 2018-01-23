#ifndef __INTERTTREEHANDLER_H__
#define __INTERTTREEHANDLER_H__

#include "LLCVBase/llcv_base.h"
#include "InterTTreeSpec.h"

namespace llcv {

  class InterTTreeHandler : public llcv_base {
  public:

  InterTTreeHandler(const std::string& fname,
		    const std::string& tname,
		    std::string name="InterTTreeHandler")
    :  llcv_base(name)
      ,_name(name)
      ,_tchain(tname.c_str())
      ,_centry(kINVALID_SIZE)
      {
	_tchain.Add(fname.c_str());
	_nentries = (size_t)_tchain.GetEntries();
      }
    
    ~InterTTreeHandler() {}
    
  private:
    std::string _name;
    TChain _tchain;
    size_t _nentries;
    size_t _centry;

    InterTTreeSpec _spec;

  public:
    bool Initialize();
    bool Next();


  };
}


#endif
