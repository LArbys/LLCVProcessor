#ifndef __SELTRACKDIR_CXX__
#define __SELTRACKDIR_CXX__

#include "SelTrackDir.h"
#include "TrackHitSorter/TrackHitSorter.h"


namespace llcv {

  void SelTrackDir::Configure (const larcv::PSet &pset) {
    set_verbosity((msg::Level_t)pset.get<int>("Verbosity",2));
    LLCV_DEBUG() << "start" << std::endl;

    fmax_hit_radius = pset.get<float>("MaxHitRadius");
    fplane          = pset.get<size_t>("Plane",2);
    fedge_length    = pset.get<float>("MaxEdgeLength",50);
    
    auto tradius = pset.get<float>("TruncatedRadius",3);
    ftsigma  = pset.get<float>("TruncatedSigma",0.5);

    _TruncMean.setRadius(tradius);

    LLCV_DEBUG() << "end" << std::endl;
  }

  void SelTrackDir::Initialize() {
    LLCV_DEBUG() << "start" << std::endl;
    
    fouttree = new TTree("trackdir","");
    AttachRSEV(fouttree);
    fouttree->Branch("trk_dedx_vv"              , &trk_dedx_vv);
    fouttree->Branch("trk_tdedx_vv"             , &trk_tdedx_vv);
    fouttree->Branch("trk_range_vv"             , &trk_range_vv);
    fouttree->Branch("trk_length_v"             , &trk_length_v);
    fouttree->Branch("trk_npts_v"               , &trk_npts_v);
    fouttree->Branch("trk_mean_dedx_v"          , &trk_mean_dedx_v);
    fouttree->Branch("trk_slope_dedx_v"         , &trk_slope_dedx_v);
    fouttree->Branch("trk_median_dedx_v"        , &trk_median_dedx_v);
    fouttree->Branch("trk_tmean_dedx_v"         , &trk_tmean_dedx_v);
    fouttree->Branch("trk_tslope_dedx_v"        , &trk_tslope_dedx_v);
    fouttree->Branch("trk_tmedian_dedx_v"       , &trk_tmedian_dedx_v);
    fouttree->Branch("trk_start_mean_dedx_v"    , &trk_start_mean_dedx_v);
    fouttree->Branch("trk_start_slope_dedx_v"   , &trk_start_slope_dedx_v);
    fouttree->Branch("trk_start_median_dedx_v"  , &trk_start_median_dedx_v);
    fouttree->Branch("trk_start_tmean_dedx_v"   , &trk_start_tmean_dedx_v);
    fouttree->Branch("trk_start_tslope_dedx_v"  , &trk_start_tslope_dedx_v);
    fouttree->Branch("trk_start_tmedian_dedx_v" , &trk_start_tmedian_dedx_v);
    fouttree->Branch("trk_middle_mean_dedx_v"   , &trk_middle_mean_dedx_v);
    fouttree->Branch("trk_middle_slope_dedx_v"  , &trk_middle_slope_dedx_v);
    fouttree->Branch("trk_middle_median_dedx_v" , &trk_middle_median_dedx_v);
    fouttree->Branch("trk_middle_tmean_dedx_v"  , &trk_middle_tmean_dedx_v);
    fouttree->Branch("trk_middle_tslope_dedx_v" , &trk_middle_tslope_dedx_v);
    fouttree->Branch("trk_middle_tmedian_dedx_v", &trk_middle_tmedian_dedx_v);
    fouttree->Branch("trk_end_mean_dedx_v"      , &trk_end_mean_dedx_v);
    fouttree->Branch("trk_end_slope_dedx_v"     , &trk_end_slope_dedx_v);
    fouttree->Branch("trk_end_median_dedx_v"    , &trk_end_median_dedx_v);
    fouttree->Branch("trk_end_tmean_dedx_v"     , &trk_end_tmean_dedx_v);
    fouttree->Branch("trk_end_tslope_dedx_v"    , &trk_end_tslope_dedx_v);
    fouttree->Branch("trk_end_tmedian_dedx_v"   , &trk_end_tmedian_dedx_v);

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

    ResizeTree(trk_v.size());
    
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
     trk_dedx_v.resize(dedx_track.size(),-1);

      static std::vector<float> dedx_v;
      static std::vector<float> range_v;
      dedx_v.clear();
      range_v.clear();
      dedx_v.reserve(dedx_track.size());
      range_v.reserve(dedx_track.size());

      auto& length = trk_length_v[ithsort];
      auto& npts   = trk_npts_v[ithsort];


      for (size_t ipt=0; ipt<dedx_track.size(); ipt++) {
	if (ipt >= bincenter_xyz.size()) continue;
	auto dedx = dedx_track.at(ipt);
	auto dx = 0.5*(float)(ipt+1);
	
	trk_dedx_v[ipt] = dedx;

	dedx_v.push_back(dedx);
	range_v.push_back(dx);
	length += dx;
      }

      npts = (int)range_v.size();

      static std::vector<float> tdedx_v;
      tdedx_v.clear();
      tdedx_v.reserve(range_v.size());

      _TruncMean.CalcTruncMeanProfile(range_v, dedx_v, tdedx_v, ftsigma);

      auto& trk_tdedx_v = trk_tdedx_vv[ithsort];
      auto& trk_range_v = trk_range_vv[ithsort];
      trk_tdedx_v = tdedx_v;
      trk_range_v = range_v;

      //
      // calculate overall 
      //
      
      auto& trk_mean_dedx = trk_mean_dedx_v[ithsort];
      auto& trk_slope_dedx = trk_slope_dedx_v[ithsort];
      auto& trk_median_dedx = trk_median_dedx_v[ithsort];

      auto& trk_tmean_dedx = trk_tmean_dedx_v[ithsort];
      auto& trk_tslope_dedx = trk_tslope_dedx_v[ithsort];
      auto& trk_tmedian_dedx = trk_tmedian_dedx_v[ithsort];
      
      trk_mean_dedx   = _TruncMean.Mean(dedx_v);
      trk_slope_dedx  = _TruncMean.Slope(range_v,dedx_v);
      trk_median_dedx = _TruncMean.Median(dedx_v);

      trk_tmean_dedx   = _TruncMean.Mean(tdedx_v);
      trk_tslope_dedx  = _TruncMean.Slope(range_v,tdedx_v);
      trk_tmedian_dedx = _TruncMean.Median(tdedx_v);


      //
      // start, middle, end 
      //

      size_t s0, s1;
      size_t m0, m1;
      size_t e0, e1;

      s0 = s1 = kINVALID_SIZE;
      m0 = m1 = kINVALID_SIZE;
      e0 = e1 = kINVALID_SIZE;

      // indexed range
      float range_sz   = (float)range_v.size();
      float range_half = range_sz / 2.0;

      // physical range
      float range = range_sz * 0.5;

      // indexed edge
      float fedge_sz  = fedge_length / 0.5;
      float fedge_half= fedge_sz / 2.0;

      LLCV_DEBUG() << "range_sz=" << range_sz << " range_half=" << range_half << std::endl;
      LLCV_DEBUG() << "range=" << range << " fedge_sz=" << fedge_sz << std::endl;
      LLCV_DEBUG() << "fedge_half=" << fedge_half << " fedge_length=" << fedge_length << std::endl;

      if (range >= (3 * fedge_length)) {
	LLCV_DEBUG() << "case0" << std::endl;
	// 50 cm edges & middle
	s0 = 0;
	s1 = (size_t) (fedge_sz - 1);
	m0 = (size_t) (range_half) - (size_t)(fedge_half);
	m1 = (size_t) (range_half) + (size_t)(fedge_half) - 1;
	e0 = e1 - (size_t)(fedge_sz + 1);
	e1 = dedx_v.size() - 1;
      }
      else {
	LLCV_DEBUG() << "case1" << std::endl;
	// just chop it into 3 parts
	float range_sz_3 = range_sz / 3.0;
	s0 = 0;
	s1 = (size_t) range_sz_3 - 1;
	m0 = s1 + 1;
	m1 = m0 + (size_t) range_sz_3 - 1;
	e0 = m1 + 1;
	e1 = dedx_v.size() - 1;
      }

      LLCV_DEBUG() << " s0=" << s0 << " s1=" << s1 
		   << " m0=" << m0 << " m1=" << m1 
		   << " e0=" << e0 << " e1=" << e1 << std::endl;

      static std::vector<float> start_dedx_v;
      static std::vector<float> start_tdedx_v;
      static std::vector<float> start_range_v;
      start_dedx_v.clear();
      start_tdedx_v.clear();
      start_range_v.clear();
      start_dedx_v.reserve(s1-s0);
      start_tdedx_v.reserve(s1-s0);
      start_range_v.reserve(s1-s0);

      static std::vector<float> middle_dedx_v;
      static std::vector<float> middle_tdedx_v;
      static std::vector<float> middle_range_v;
      middle_dedx_v.clear();
      middle_tdedx_v.clear();
      middle_range_v.clear();
      middle_dedx_v.reserve(m1-m0);
      middle_tdedx_v.reserve(m1-m0);
      middle_range_v.reserve(m1-m0);

      static std::vector<float> end_dedx_v;
      static std::vector<float> end_tdedx_v;
      static std::vector<float> end_range_v;
      end_dedx_v.clear();
      end_tdedx_v.clear();
      end_range_v.clear();
      end_dedx_v.reserve(e1-e0);
      end_tdedx_v.reserve(e1-e0);
      end_range_v.reserve(e1-e0);

      for(size_t id=s0; id<=s1; ++id) {
	start_dedx_v.push_back(dedx_v[id]);
	start_tdedx_v.push_back(tdedx_v[id]);
	start_range_v.push_back(range_v[id]);
      }

      for(size_t id=m0; id<=m1; ++id) {
	middle_dedx_v.push_back(dedx_v[id]);
	middle_tdedx_v.push_back(tdedx_v[id]);
	middle_range_v.push_back(range_v[id]);
      }
      
      for(size_t id=e0; id<=e1; ++id) {
	end_dedx_v.push_back(dedx_v[id]);
	end_tdedx_v.push_back(tdedx_v[id]);
	end_range_v.push_back(range_v[id]);
      }

      // start
      auto& trk_start_mean_dedx = trk_start_mean_dedx_v[ithsort];
      auto& trk_start_slope_dedx = trk_start_slope_dedx_v[ithsort];
      auto& trk_start_median_dedx = trk_start_median_dedx_v[ithsort];

      auto& trk_start_tmean_dedx = trk_start_tmean_dedx_v[ithsort];
      auto& trk_start_tslope_dedx = trk_start_tslope_dedx_v[ithsort];
      auto& trk_start_tmedian_dedx = trk_start_tmedian_dedx_v[ithsort];
      
      trk_start_mean_dedx   = _TruncMean.Mean(start_dedx_v);
      trk_start_slope_dedx  = _TruncMean.Slope(start_range_v,start_dedx_v);
      trk_start_median_dedx = _TruncMean.Median(start_dedx_v);

      trk_start_tmean_dedx   = _TruncMean.Mean(start_tdedx_v);
      trk_start_tslope_dedx  = _TruncMean.Slope(start_range_v,start_tdedx_v);
      trk_start_tmedian_dedx = _TruncMean.Median(start_tdedx_v);

      // middle
      auto& trk_middle_mean_dedx = trk_middle_mean_dedx_v[ithsort];
      auto& trk_middle_slope_dedx = trk_middle_slope_dedx_v[ithsort];
      auto& trk_middle_median_dedx = trk_middle_median_dedx_v[ithsort];

      auto& trk_middle_tmean_dedx = trk_middle_tmean_dedx_v[ithsort];
      auto& trk_middle_tslope_dedx = trk_middle_tslope_dedx_v[ithsort];
      auto& trk_middle_tmedian_dedx = trk_middle_tmedian_dedx_v[ithsort];
      
      trk_middle_mean_dedx   = _TruncMean.Mean(middle_dedx_v);
      trk_middle_slope_dedx  = _TruncMean.Slope(middle_range_v,middle_dedx_v);
      trk_middle_median_dedx = _TruncMean.Median(middle_dedx_v);

      trk_middle_tmean_dedx   = _TruncMean.Mean(middle_tdedx_v);
      trk_middle_tslope_dedx  = _TruncMean.Slope(middle_range_v,middle_tdedx_v);
      trk_middle_tmedian_dedx = _TruncMean.Median(middle_tdedx_v);

      // end
      auto& trk_end_mean_dedx = trk_end_mean_dedx_v[ithsort];
      auto& trk_end_slope_dedx = trk_end_slope_dedx_v[ithsort];
      auto& trk_end_median_dedx = trk_end_median_dedx_v[ithsort];

      auto& trk_end_tmean_dedx = trk_end_tmean_dedx_v[ithsort];
      auto& trk_end_tslope_dedx = trk_end_tslope_dedx_v[ithsort];
      auto& trk_end_tmedian_dedx = trk_end_tmedian_dedx_v[ithsort];
      
      trk_end_mean_dedx   = _TruncMean.Mean(end_dedx_v);
      trk_end_slope_dedx  = _TruncMean.Slope(end_range_v,end_dedx_v);
      trk_end_median_dedx = _TruncMean.Median(end_dedx_v);

      trk_end_tmean_dedx   = _TruncMean.Mean(end_tdedx_v);
      trk_end_tslope_dedx  = _TruncMean.Slope(end_range_v,end_tdedx_v);
      trk_end_tmedian_dedx = _TruncMean.Median(end_tdedx_v);


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

  void SelTrackDir::ResizeTree(size_t ntracks) {
    
    trk_dedx_vv.resize(ntracks);
    trk_tdedx_vv.resize(ntracks);
    trk_range_vv.resize(ntracks);

    trk_length_v.resize(ntracks);
    trk_npts_v.resize(ntracks);

    trk_mean_dedx_v.resize(ntracks);
    trk_slope_dedx_v.resize(ntracks);
    trk_median_dedx_v.resize(ntracks);

    trk_tmean_dedx_v.resize(ntracks);
    trk_tslope_dedx_v.resize(ntracks);
    trk_tmedian_dedx_v.resize(ntracks);

    trk_start_mean_dedx_v.resize(ntracks);
    trk_start_slope_dedx_v.resize(ntracks);
    trk_start_median_dedx_v.resize(ntracks);

    trk_start_tmean_dedx_v.resize(ntracks);
    trk_start_tslope_dedx_v.resize(ntracks);
    trk_start_tmedian_dedx_v.resize(ntracks);

    trk_middle_mean_dedx_v.resize(ntracks);
    trk_middle_slope_dedx_v.resize(ntracks);
    trk_middle_median_dedx_v.resize(ntracks);

    trk_middle_tmean_dedx_v.resize(ntracks);
    trk_middle_tslope_dedx_v.resize(ntracks);
    trk_middle_tmedian_dedx_v.resize(ntracks);

    trk_end_mean_dedx_v.resize(ntracks);
    trk_end_slope_dedx_v.resize(ntracks);
    trk_end_median_dedx_v.resize(ntracks);

    trk_end_tmean_dedx_v.resize(ntracks);
    trk_end_tslope_dedx_v.resize(ntracks);
    trk_end_tmedian_dedx_v.resize(ntracks);

  }

  void SelTrackDir::ResetTree() {

    trk_dedx_vv.clear();
    trk_tdedx_vv.clear();
    trk_range_vv.clear();

    trk_length_v.clear();
    trk_npts_v.clear();

    trk_mean_dedx_v.clear();
    trk_slope_dedx_v.clear();
    trk_median_dedx_v.clear();

    trk_tmean_dedx_v.clear();
    trk_tslope_dedx_v.clear();
    trk_tmedian_dedx_v.clear();

    trk_start_mean_dedx_v.clear();
    trk_start_slope_dedx_v.clear();
    trk_start_median_dedx_v.clear();

    trk_start_tmean_dedx_v.clear();
    trk_start_tslope_dedx_v.clear();
    trk_start_tmedian_dedx_v.clear();

    trk_middle_mean_dedx_v.clear();
    trk_middle_slope_dedx_v.clear();
    trk_middle_median_dedx_v.clear();

    trk_middle_tmean_dedx_v.clear();
    trk_middle_tslope_dedx_v.clear();
    trk_middle_tmedian_dedx_v.clear();

    trk_end_mean_dedx_v.clear();
    trk_end_slope_dedx_v.clear();
    trk_end_median_dedx_v.clear();

    trk_end_tmean_dedx_v.clear();
    trk_end_tslope_dedx_v.clear();
    trk_end_tmedian_dedx_v.clear();

    return;
  }

}


#endif
  
