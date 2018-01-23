#ifndef __INTERTTREEMANAGER_H__
#define __INTERTTREEMANAGER_H__

#include "LLCVBase/llcv_base.h"
#include "InterTTreeSpec.h"

namespace llcv {

  class InterTTreeManager : public llcv_base {

  public:
    
  InterTTreeManager(std::string name="InterTTreeManager")
    :  llcv_base(name)
      , _name(name)
      , _tchain(nullptr)
      , _centry(kINVALID_SIZE)
    {}
    
  InterTTreeManager(const std::string& fname,
		    const std::string& tname,
		    std::string name="InterTTreeManager")
    :  llcv_base(name)
      , _name(name)
      , _tchain(nullptr)
      , _centry(kINVALID_SIZE)
    {
      Initialize(fname,tname);
    }
    
    ~InterTTreeManager() {}
    
  private:
    std::string _name;
    TChain* _tchain;
    size_t _nentries;
    size_t _centry;

    InterTTreeSpec _spec;

  public:
    void Initialize(const std::string& fname, const std::string& tname);
    bool Next();


  };
}


#endif
