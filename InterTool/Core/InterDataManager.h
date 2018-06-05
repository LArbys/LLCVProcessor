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
#include "DataFormat/opdetwaveform.h"

//lcv
#include "DataFormat/PGraph.h"
#include "DataFormat/Pixel2DCluster.h"
#include "DataFormat/ImageMeta.h"

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
    std::vector<const larlite::opdetwaveform*> _opdigit_v;
  
    // larcv
    const larcv::PGraph* _pgraph;
    std::vector<std::vector<larcv::Pixel2DCluster> > _pcluster_vv;

    // asses
    std::vector<std::vector<size_t> > _ass_shower_to_cluster_vv;
    std::vector<std::vector<size_t> > _ass_cluster_to_hit_vv;

    //getters
  public:

    // larlite
    const larlite::vertex* Vertex() const { return _vertex; }

    const std::vector<const larlite::shower*       >& Showers()  const { return _shower_v;  }
    const std::vector<const larlite::track*        >& Tracks()   const { return _track_v;   }
    const std::vector<const larlite::opflash*      >& Flashes()  const { return _opflash_v; }
    const std::vector<const larlite::cluster*      >& Clusters() const { return _cluster_v; }
    const std::vector<const larlite::hit*          >& Hits()     const { return _hit_v;     }
    const std::vector<const larlite::opdetwaveform*>& Digits()   const { return _opdigit_v; }

    //larcv
    const larcv::PGraph* PGraph() const { return _pgraph; }

    const std::vector<std::vector<larcv::Pixel2DCluster> >& Particles() const { return _pcluster_vv; }

    //writeout
  public:
    
    larlite::track& MakeTrack();
    larlite::shower& MakeShower();

    const std::vector<larlite::track>& OutputTracks() const { return _out_track_v; }
    const std::vector<larlite::shower>& OutputShowers() const { return _out_shower_v; }
    
  private:
    std::vector<larlite::track> _out_track_v;
    std::vector<larlite::shower> _out_shower_v;
    

  };

}

#endif
