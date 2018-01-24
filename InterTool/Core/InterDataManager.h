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

  class InterDriver;

  class InterDataManager {
    
  public:
    friend class InterDriver;
    
    InterDataManager() {}
    ~InterDataManager() {}
  
  private:
    // id
    size_t _id;
  
    // larlite
    const larlite::vertex* _vertex;
  
    std::vector<const larlite::track*> _track_v;
    std::vector<const larlite::shower*> _shower_v;
    std::vector<const larlite::cluster*>  _cluster_v;
    std::vector<const larlite::hit*> _hit_v;
  
    std::vector<const larlite::opflash*> _opflash_v;
  
    // larcv
    const larcv::PGraph* _pgraph;
    std::map<larcv::PlaneID_t, std::vector<larcv::Pixel2DCluster> >* _pix_m;
  
    // asses
    std::vector<std::vector<size_t> > _ass_shower_to_cluster_vv;
    std::vector<std::vector<size_t> > _ass_cluster_to_hit_vv;


  public:
    const larlite::vertex* Vertex() { return _vertex; }
    const std::vector<const larlite::shower*  >& Showers()  { return _shower_v; }
    const std::vector<const larlite::track*   >& Tracks()   { return _track_v;  }
    const std::vector<const larlite::opflash* >& Flashes()  { return _opflash_v;}
    const std::vector<const larlite::cluster* >& Clusters() { return _cluster_v;}
    const std::vector<const larlite::hit*     >& Hits()     { return _hit_v;    }

    

  };

}

#endif
