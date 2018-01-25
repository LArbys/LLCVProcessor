#ifndef __INTERTTREEMANAGER_H__
#define __INTERTTREEMANAGER_H__

#include "LLCVBase/llcv_base.h"
#include "Base/PSet.h"
#include "InterTTreeSpec.h"


namespace llcv {

  class InterModule;
  class InterDriver;

  class InterTTreeManager : public llcv_base {

    friend class InterModule;
    friend class InterDriver;

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

  public:
    
    template <class T> T Scalar(const std::string& str) const;
    
    std::vector<float> Vector(const std::string& str) const;

    std::vector<std::vector<float> > VVector(const std::string& str) const;
    
    
  private:
    std::string _name;

    TChain* _tchain;

    size_t _nentries;
    size_t _centry;

    InterTTreeSpec _spec;

    void Configure(const larcv::PSet& cfg);
    void Initialize(const std::string& fname, const std::string& tname);

    bool Next();

    size_t Run()    const { return (size_t)(*(_spec.run)); }
    size_t SubRun() const { return (size_t)(*(_spec.subrun)); }
    size_t Event()  const { return (size_t)(*(_spec.event)); }
    size_t Vertex() const { return (size_t)(*(_spec.vtxid)); }


  };
}


#endif
