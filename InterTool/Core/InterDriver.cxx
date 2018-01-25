#ifndef __INTERDRIVER_CXX__
#define __INTERDRIVER_CXX__

#include "InterDriver.h"


namespace llcv {

  void InterDriver::Configure(const larcv::PSet& cfg) {
    set_verbosity((msg::Level_t)cfg.get<int>("Verbosity",2));
    LLCV_DEBUG() << "start" << std::endl;
    LLCV_DEBUG() << "end" << std::endl;
  }

  void InterDriver::Initialize() {
    LLCV_DEBUG() << "start" << std::endl;

    _fout = TFile::Open("out.root","RECREATE"); 

    for(auto selptr : _sel_base_v) {
      selptr->set_output_file(_fout);
      selptr->Initialize();
    }

    LLCV_DEBUG() << "end" << std::endl;
  }

  void InterDriver::Process() {
    LLCV_DEBUG() << "start" << std::endl;
    
    LLCV_DEBUG() << "Driver@(r,s,e)=(" << _run << "," << _subrun << "," << _event << ")" << std::endl;

    std::vector<std::vector<double> > score_vv;
    score_vv.resize(_data_mgr_v.size());

    for(size_t vtxid=0; vtxid<_data_mgr_v.size(); ++vtxid) {

      assert(_run    == _tree_mgr.Run());
      assert(_subrun == _tree_mgr.SubRun());
      assert(_event  == _tree_mgr.Event());
      assert(vtxid   == _tree_mgr.Vertex());
      
      auto& score_v = score_vv[vtxid];
      score_v.resize(_sel_base_v.size(),kINVALID_FLOAT);
      auto& data_mgr = _data_mgr_v[vtxid];


      // prepare the image
      _img_mgr.SetVertex(data_mgr.Vertex()->X(),
			 data_mgr.Vertex()->Y(),
			 data_mgr.Vertex()->Z());


      for(size_t selid=0; selid<_sel_base_v.size(); ++selid) {
	auto sel_base = _sel_base_v[selid];
	sel_base->_data_mgr_ptr = &data_mgr;
	sel_base->_img_mgr_ptr  = &_img_mgr;
	sel_base->_tree_mgr_ptr = &_tree_mgr;

	score_v[selid] = sel_base->Select();
      }
      
      _tree_mgr.Next();
    }
    
    LLCV_DEBUG() << "end" << std::endl;
  }

  void InterDriver::Finalize() {
    LLCV_DEBUG() << "start" << std::endl;

    for(auto selptr : _sel_base_v)
      selptr->Finalize();

    _fout->Close();

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

    if (vtxid>=_data_mgr_v.size()) 
      throw llcv_err("Requested vtxid is out of range");

    _data_mgr_v[vtxid]._pgraph = pgraph;
    return id;
  }

  size_t InterDriver::AttachOpFlash(size_t vtxid, const larlite::opflash* opflash) {
    size_t id = kINVALID_SIZE;

    if (vtxid>=_data_mgr_v.size()) 
      throw llcv_err("Requested vtxid is out of range");

    id = _data_mgr_v[vtxid]._opflash_v.size();
    _data_mgr_v[vtxid]._opflash_v.resize(id+1);
    _data_mgr_v[vtxid]._opflash_v[id] = opflash;

    return id;
  }
  
  size_t InterDriver::AttachTrack(size_t vtxid, const larlite::track* track) {
    size_t id = kINVALID_SIZE;

    if (vtxid>=_data_mgr_v.size()) 
      throw llcv_err("Requested vtxid is out of range");

    id = _data_mgr_v[vtxid]._track_v.size();
    _data_mgr_v[vtxid]._track_v.resize(id+1);
    _data_mgr_v[vtxid]._track_v[id] = track;
    return id;
  }

  size_t InterDriver::AttachShower(size_t vtxid, const larlite::shower* shower) {
    size_t id = kINVALID_SIZE;

    if (vtxid>=_data_mgr_v.size()) 
      throw llcv_err("Requested vtxid is out of range");

    id = _data_mgr_v[vtxid]._shower_v.size();
    _data_mgr_v[vtxid]._shower_v.resize(id+1);
    _data_mgr_v[vtxid]._shower_v[id] = shower;
    _data_mgr_v[vtxid]._ass_shower_to_cluster_vv.resize(id+1);
    return id;
  }

  size_t InterDriver::AttachCluster(size_t vtxid, size_t shrid, const larlite::cluster* cluster) {
    size_t id = kINVALID_SIZE;

    if (vtxid>=_data_mgr_v.size()) 
      throw llcv_err("Requested vtxid is out of range");

    if (shrid>=_data_mgr_v[vtxid]._shower_v.size())
      throw llcv_err("Requested shrid is out of range");

    if (shrid>=_data_mgr_v[vtxid]._ass_shower_to_cluster_vv.size())
      throw llcv_err("Requested shrid is out of range");

    id = _data_mgr_v[vtxid]._cluster_v.size();
    _data_mgr_v[vtxid]._cluster_v.resize(id+1);
    _data_mgr_v[vtxid]._cluster_v[id] = cluster;
    _data_mgr_v[vtxid]._ass_shower_to_cluster_vv[shrid].push_back(id);
    _data_mgr_v[vtxid]._ass_cluster_to_hit_vv.resize(id+1);
    return id;
  }

  size_t InterDriver::AttachHit(size_t vtxid, size_t cluid, const larlite::hit* hit) {
    size_t id = kINVALID_SIZE;

    if (vtxid>=_data_mgr_v.size()) 
      throw llcv_err("Requested vtxid is out of range");

    if (cluid>=_data_mgr_v[vtxid]._cluster_v.size())
      throw llcv_err("Requested cluid is out of range");

    if (cluid>=_data_mgr_v[vtxid]._ass_cluster_to_hit_vv.size())
      throw llcv_err("Requested cluid is out of range");

    id = _data_mgr_v[vtxid]._hit_v.size();
    _data_mgr_v[vtxid]._hit_v.resize(id+1);
    _data_mgr_v[vtxid]._hit_v[id] = hit;
    _data_mgr_v[vtxid]._ass_cluster_to_hit_vv[cluid].push_back(id);
    return id;
  }
  
  void InterDriver::AttachImage(const std::vector<larcv::Image2D>& img_v, InterImageType itype)
    { _img_mgr.SetImage(img_v,itype); }

  void InterDriver::AttachInterFile(const std::string& fname,const std::string& tname)
  { _tree_mgr.Initialize(fname,tname); }

  void InterDriver::AddSelection(InterSelBase* sbase) 
  { _sel_base_v.push_back(sbase); }

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
