#ifndef __SELTRACKDIR_CXX__
#define __SELTRACKDIR_CXX__

#include "SelTrackDir.h"
#include "TrackHitSorter/TrackHitSorter.h"

namespace llcv {

  void SelTrackDir::Configure (const larcv::PSet &pset) {
    set_verbosity((msg::Level_t)pset.get<int>("Verbosity",2));
    LLCV_DEBUG() << "start" << std::endl;

    fmax_hit_radius = pset.get<float>("MaxHitRadius");

    LLCV_DEBUG() << "end" << std::endl;
  }

  void SelTrackDir::Initialize() {
    LLCV_DEBUG() << "start" << std::endl;

    fouttree = new TTree("trackdir","");
    AttachRSEV(fouttree);
    fouttree->Branch("track_dedx_vv",&trk_dedx_vv);

    LLCV_DEBUG() << "end" << std::endl;
    return;
  }

  double SelTrackDir::Select() {
    LLCV_DEBUG() << "start" << std::endl;
    LLCV_DEBUG() << "=======================" << std::endl;

    ResetTree();

    const larlite::vertex& vtx = *(Data().Vertex());
    const auto& phit_v = Data().Hits();

    std::vector<larlite::hit> hit_v;
    hit_v.reserve(phit_v.size());
    for (auto const& phit : phit_v ) 
      hit_v.push_back(*phit);

    std::vector<int> hitmask_v(hit_v.size(),1);
    LLCV_DEBUG() << "number of hits: " << hit_v.size() << std::endl;
    LLCV_DEBUG() << "Number of tracks: "  << Data().Tracks().size() << std::endl;    

    const auto& trk_v = Data().Tracks();
    std::vector<std::vector<float>> dedx_track_per_plane;

    fplane = 2;

    trk_dedx_vv.resize(trk_v.size());
    
    for(size_t ithsort=0; ithsort<trk_v.size(); ++ithsort) { 

      const auto& lltrack = *(trk_v[ithsort]);
      larlitecv::TrackHitSorter trackhitsort;
      trackhitsort.buildSortedHitList(vtx, lltrack, hit_v, fmax_hit_radius, hitmask_v);

      dedx_track_per_plane.clear();
      dedx_track_per_plane.resize(3);
      
      trackhitsort.getPathBinneddEdx(0.5,0.5,dedx_track_per_plane);
      
      const auto& bincenter_xyz = trackhitsort.getBinCentersXYZ(fplane); 
      const auto& dedx_track = dedx_track_per_plane.at(fplane);
      
      auto& trk_dedx_v = trk_dedx_vv[ithsort];
      trk_dedx_v.resize(dedx_track.size(),0.0);
      
      for (size_t ipt=0; ipt<dedx_track.size(); ipt++) {
	if (ipt >= bincenter_xyz.size()) continue;
	trk_dedx_v[ipt] = dedx_track.at(ipt);
      }

    }
    
    fouttree->Fill();

    LLCV_DEBUG() << "=======================" << std::endl;
    LLCV_DEBUG() << "end" << std::endl;
    return 0.0;
  }
  
  void SelTrackDir::Finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    fouttree->Write();
    LLCV_DEBUG() << "end" << std::endl;
    return;
  }

  void SelTrackDir::ResetTree() {
    trk_dedx_vv.clear();
    return;
  }

}


#endif
  
