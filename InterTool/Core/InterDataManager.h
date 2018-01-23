#ifndef __INTERDATAMANAGER_H__
#define __INTERDATAMANAGER_H__

#include "LLCVBase/llcv_err.h"
#include "LLCVBase/llcv_base.h"


//ll
#include "DataFormat/vertex.h"
#include "DataFormat/track.h"
#include "DataFormat/shower.h"
#include "DataFormat/cluster.h"
#include "DataFormat/hit.h"
#include "DataFormat/opflash.h"

//lcv
#include "DataFormat/PGraph.h"
#include "DataFormat/Pixel2DCluster.h"

namespace llcv {

  class InterDataManager : public llcv_base {
    
  public:
  InterDataManager(std::string name="InterDataManager") : 
    llcv_base(name),
      _name(name) {}
    
    ~InterDataManager() {}
    
    
  private:
    std::string _name;

    // larlite
    larlite::vertex* vertex;

    std::vector<larlite::track*> track_v;

    std::vector<larlite::shower*> shower_v;
    std::vector<larlite::cluster*> cluster_v;
    std::vector<larlite::hit*> hit_v;

    std::vector<larlite::opflash*> opflash_v;
    
    // larcv
    std::vector<larcv::PGraph*> pgraph_v;
    std::map<larcv::PlaneID_t, std::vector<larcv::Pixel2DCluster> >* pix_m;
    std::map<larcv::PlaneID_t, std::vector<larcv::ImageMeta> >* pix_meta_m;
    

  };

}

#endif
