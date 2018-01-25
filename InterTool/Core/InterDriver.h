#ifndef __INTERDRIVER_H__
#define __INTERDRIVER_H__

#include "InterAnaBase.h"
#include "InterSelBase.h"

#include "InterTTreeManager.h"
#include "InterDataManager.h"
#include "InterImageManager.h"

#include "InterToolTypes.h"

namespace llcv {

  class InterModule;
  
  class InterDriver : public llcv_base {

    friend class InterModule;

  public:
  
  InterDriver(std::string name="InterDriver")
    :   llcv_base(name)
    {}

    ~InterDriver() {}

  public:

    void Configure(const larcv::PSet& cfg);
    void Initialize();
    void Process();
    void Finalize();
    
  private:

    size_t _run;
    size_t _subrun;
    size_t _event;
    
    // Next() for next vertex
    InterTTreeManager _tree_mgr;

    // data manager per vertex
    std::vector<InterDataManager> _data_mgr_v;

    // image manager per event
    InterImageManager _img_mgr;

    // analysis to be run per vertex or per event
    std::vector<InterSelBase*> _sel_base_v;
    
  public:

    //
    // Algorithm attachment
    //
    void AddSelection(InterSelBase* sbase) 
    { _sel_base_v.push_back(sbase); }

    
    //
    // InterModule attachments
    //
    void AttachImage(const std::vector<larcv::Image2D>& img_v, InterImageType itype)
    { _img_mgr.SetImage(img_v,itype); }
    
    size_t AttachVertex (const larlite::vertex* vertex);
    size_t AttachPGraph (size_t vtxid, const larcv::PGraph* pgraph);
    size_t AttachOpFlash(size_t vtxid, const larlite::opflash* opflash);
    size_t AttachTrack  (size_t vtxid, const larlite::track* track);
    size_t AttachShower (size_t vtxid, const larlite::shower* shower);
    size_t AttachCluster(size_t vtxid, size_t shrid, const larlite::cluster* cluster);
    size_t AttachHit    (size_t vtxid, size_t cluid, const larlite::hit* hit);

    void AttachInterFile(const std::string& fname,const std::string& tname)
    { _tree_mgr.Initialize(fname,tname); }


    //
    //
    //
    void Reset();
    void Dump();


  };
  
  
}

#endif
