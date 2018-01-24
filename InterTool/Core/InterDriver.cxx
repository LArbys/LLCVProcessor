#ifndef __INTERDRIVER_CXX__
#define __INTERDRIVER_CXX__

#include "InterDriver.h"

namespace llcv {

  void InterDriver::Configure(const larcv::PSet& cfg) {
    LLCV_DEBUG() << "start" << std::endl;

    set_verbosity((msg::Level_t)cfg.get<int>("Verbosity",2));

    
    
    LLCV_DEBUG() << "end" << std::endl;
  }


  void InterDriver::Process() {
    LLCV_DEBUG() << "start" << std::endl;
    //
    // select best candidate vertex per event
    //
    
    

    
    LLCV_DEBUG() << "end" << std::endl;
  }

  
  size_t InterDriver::AttachVertex(const larlite::vertex* vertex) {
    size_t id = kINVALID_SIZE;
    id = _data_mgr_v.size();
    _data_mgr_v.resize(id+1);
    _data_mgr_v[id]._vertex = vertex;
    return id;
  }
  
  size_t InterDriver::AttachPGraph(size_t vtxid, const larcv::PGraph* pgraph) {
    size_t id = kINVALID_SIZE;
    if (vtxid>=_data_mgr_v.size()) throw llcv_err("Requested vtxid is out of range");
    _data_mgr_v[vtxid]._pgraph = pgraph;
    return id;
  }
  
  size_t InterDriver::AttachTrack(size_t vtxid, const larlite::track* track) {
    size_t id = kINVALID_SIZE;
    if (vtxid>=_data_mgr_v.size()) throw llcv_err("Requested vtxid is out of range");
    id = _data_mgr_v[vtxid]._track_v.size();
    _data_mgr_v[vtxid]._track_v.resize(id+1);
    _data_mgr_v[vtxid]._track_v[id] = track;
    return id;
  }

  size_t InterDriver::AttachShower(size_t vtxid, const larlite::shower* shower) {
    size_t id = kINVALID_SIZE;
    if (vtxid>=_data_mgr_v.size()) throw llcv_err("Requested vtxid is out of range");
    id = _data_mgr_v[vtxid]._shower_v.size();
    _data_mgr_v[vtxid]._shower_v.resize(id+1);
    _data_mgr_v[vtxid]._shower_v[id] = shower;
    return id;
  }

  size_t InterDriver::AttachCluster(size_t vtxid, size_t shrid, const larlite::cluster* cluster) {
    size_t id = kINVALID_SIZE;
    if (vtxid>=_data_mgr_v.size()) 
      throw llcv_err("Requested vtxid is out of range");
    if (shrid>=_data_mgr_v.at(vtxid)._shower_v.size())
      throw llcv_err("Requested shrid is out of range");
    id = _data_mgr_v[vtxid]._cluster_v.size();
    _data_mgr_v[vtxid]._cluster_v.resize(id+1);
    _data_mgr_v[vtxid]._cluster_v[id] = cluster;
    return id;
  }

  size_t InterDriver::AttachHit(size_t vtxid, size_t shrid, size_t cluid, const larlite::hit* hit) {
    size_t id = kINVALID_SIZE;
    if (vtxid>=_data_mgr_v.size()) 
      throw llcv_err("Requested vtxid is out of range");
    if (shrid>=_data_mgr_v.at(vtxid)._shower_v.size())
      throw llcv_err("Requested shrid is out of range");
    if (cluid>=_data_mgr_v.at(vtxid)._cluster_v.size())
      throw llcv_err("Requested cluid is out of range");
    id = _data_mgr_v[vtxid]._hit_v.size();
    _data_mgr_v[vtxid]._hit_v.resize(id+1);
    _data_mgr_v[vtxid]._hit_v[id] = hit;
    return id;
  }

  void InterDriver::Reset() {
    _data_mgr_v.clear();
    _run    = kINVALID_SIZE;
    _subrun = kINVALID_SIZE;
    _event  = kINVALID_SIZE;
  }


  void InterDriver::Dump() {
    std::cout << _data_mgr_v.size() << " verticies" << std::endl;

    for(size_t vtxid=0; vtxid < _data_mgr_v.size(); ++vtxid) {
      std::cout << "@vtxid=" << vtxid << std::endl;
      std::cout << _data_mgr_v[vtxid]._track_v.size() << " associated tracks" << std::endl;
      std::cout << _data_mgr_v[vtxid]._shower_v.size() << " associated showers" << std::endl;
      std::cout << _data_mgr_v[vtxid]._cluster_v.size() << " associated clusters" << std::endl;
      std::cout << _data_mgr_v[vtxid]._hit_v.size() << " associated hits" << std::endl;
    }

  }

}

#endif
