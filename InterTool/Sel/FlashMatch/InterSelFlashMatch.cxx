#ifndef __INTERSELFLASHMATCH_CXX__
#define __INTERSELFLASHMATCH_CXX__

#include "InterSelFlashMatch.h"

#include "TCanvas.h"
#include "TH1D.h"

#include <stdlib.h>

namespace llcv {

  void InterSelFlashMatch::Configure (const larcv::PSet &pset) {
    set_verbosity((msg::Level_t)pset.get<int>("Verbosity",2));
    LLCV_DEBUG() << "start" << std::endl;
    larcv::PSet genflash_pset = pset.get<larcv::PSet>("GeneralFlashMatchAlgo");
    larlitecv::GeneralFlashMatchAlgoConfig genflash_cfg = larlitecv::GeneralFlashMatchAlgoConfig::MakeConfigFromPSet( genflash_pset );
    genflashmatch = new larlitecv::GeneralFlashMatchAlgo( genflash_cfg );
    shower_correction_factor = pset.get<float>("ShowerCorrectionFactor",50.0);
    isMC = pset.get<bool>("IsMC");
    fmax_hit_radius = pset.get<float>("MaxHitRadius");
    
    LLCV_DEBUG() << "end" << std::endl;
  }

  void InterSelFlashMatch::Initialize() {
    if(!_fout) throw llcv_err("No output file?");

    _fout->cd();
    outtree = new TTree("ffmatch","Final Flash Match result");
    AttachRSEV(outtree);
    outtree->Branch("vtxpos",vtxpos,"vtxpos[3]/F");
    outtree->Branch("chi2",&best_chi2,"chi2/F");
    outtree->Branch("data_totpe",&best_data_totpe,"data_totpe/F");
    outtree->Branch("hypo_totpe",&hypo_totpe,"hypo_totpe/F");
    outtree->Branch("hypo_pe",hypo_pe,"hypo_pe[32]/F");
    outtree->Branch("data_pe",data_pe,"data_pe[32]/F");
    outtree->Branch("hypo_protonpe",hypo_protonpe,"hypo_protonpe[32]/F");
    outtree->Branch("hypo_showerpe",hypo_showerpe,"hypo_showerpe[32]/F");
    outtree->Branch("reco_nu_E", &reco_nu_E, "reco_nu_E/F");
    outtree->Branch("reco_shower_E", &reco_shower_E, "reco_shower_E/F");
    outtree->Branch("reco_proton_E", &reco_proton_E, "reco_proton_E/F");
    outtree->Branch("true_nu_E", &true_nu_E, "true_nu_E/F");
    outtree->Branch("true_shower_E", &true_shower_E, "true_shower_E/F");
    outtree->Branch("true_proton_E", &true_proton_E, "true_proton_E/F");

    outtree->Branch("valid", &valid, "valid/I");
    outtree->Branch("scedr", &scedr, "scedr/F");
  }


  double InterSelFlashMatch::Select() {
    LLCV_DEBUG() << "start" << std::endl;

    // get distance between reco vertex and true vertex
    scedr = Tree().Scalar<float>("locv_scedr");
    
    // get the reconstructed proton energy (from selection)
    auto pproton_energy = Tree().Scalar<float>("reco_LL_proton_energy");
    if (pproton_energy < 0)  {
      LLCV_INFO() << "No reconstructed proton energy" << std::endl;
      valid = 0;
      outtree->Fill();
      return 0;
    }


    // 
    // adapated from taritree's code:
    // https://goo.gl/8MboZe
    //


    const larlite::vertex& vtx = *(Data().Vertex());
    int vertexid = VertexID();

    vtxpos[0] = (float)vtx.X();
    vtxpos[1] = (float)vtx.Y();
    vtxpos[2] = (float)vtx.Z();

    valid = 1; // if we f-up marker it with this variable

    reco_nu_E     = Tree().Scalar<float>("reco_LL_total_energy");
    reco_shower_E = Tree().Scalar<float>("reco_LL_electron_energy");
    reco_proton_E = Tree().Scalar<float>("reco_LL_proton_energy");
    
    true_nu_E     = Tree().Scalar<float>("locv_energyInit");
    true_shower_E = Tree().Scalar<float>("anashr1_mc_energy");
    true_proton_E = Tree().Scalar<float>("locv_dep_sum_proton");
    
    // GET DATA
    // --------
    // get hits
    const std::vector<const larlite::hit*>& phit_v = Data().Hits();
    // unfortunately, we need a vector<larlite::hit>. we copy ... terrible
    std::vector< larlite::hit > hit_v;
    hit_v.reserve( phit_v.size() );
    for (auto const& phit : phit_v )
      hit_v.push_back( *phit );
    std::vector< int > hitmask_v( hit_v.size(), 1 );
    
    // get prong class
    auto shrid = Tree().Scalar<int>("reco_LL_electron_id");
    auto protonid = Tree().Scalar<int>("reco_LL_proton_id");

    // get shower
    const larlite::shower& shreco = *(Data().Showers().at(shrid));

    // get tracks
    const std::vector<const larlite::track*>& ptrack_v = Data().Tracks(); // vic has already filtered out tracks associated to this vertex?
    std::vector< larlitecv::TrackHitSorter > dedxgen_v(ptrack_v.size()); // this class will generated de/dx per 3d position along the track. will use to set MeV deposited at 3d pos.

    int ithsort=0;
    for ( auto const& ptrack : ptrack_v ) {
      const larlite::track& lltrack = *(ptrack);
      dedxgen_v[ithsort].buildSortedHitList( vtx, lltrack, hit_v, fmax_hit_radius, hitmask_v );
      ithsort++;
    }

    // ok, we now have dedx 3d tracks for all tracks and shower
    // we want to build 1mu1p and 1e1p hypothesis

    flashana::QCluster_t qcluster_1mu1p = build1mu1pQCluster( protonid, dedxgen_v );
    flashana::QCluster_t qcluster_1e1p  = build1e1pQCluster(  protonid, shrid, vtx, shreco, dedxgen_v );


    const auto& opflash_ptr_v = Data().Flashes();
    std::vector<larlite::opflash> ev_opflash;
    ev_opflash.reserve(opflash_ptr_v.size());
    for(const auto& opflash_ptr : opflash_ptr_v)
      ev_opflash.emplace_back(*opflash_ptr);

    std::vector<flashana::Flash_t> dataflash_v = genflashmatch->MakeDataFlashes( ev_opflash );
    
    // make flash hypothesis
    flashana::Flash_t hypo_1e1p   = genflashmatch->GenerateUnfittedFlashHypothesis( qcluster_1e1p );
    flashana::Flash_t hypo_1mu1p  = genflashmatch->GenerateUnfittedFlashHypothesis( qcluster_1mu1p );

    // make flash hist
    auto run    = Tree().Scalar<int>("run");
    auto subrun = Tree().Scalar<int>("subrun");
    auto event  = Tree().Scalar<int>("event");

    std::stringstream hname_hypo_1mu1p;
    hname_hypo_1mu1p << "hflash_" << run << "_" << event << "_" << subrun << "_vtx" << vertexid << "_1mu1p";
    std::stringstream hname_hypo_1e1p;
    hname_hypo_1e1p << "hflash_" << run << "_" << event << "_" << subrun << "_vtx" << vertexid << "_1e1p";
    
    TH1D flashhist_hypo_1mu1p( hname_hypo_1mu1p.str().c_str(),"",32,0,32);
    TH1D flashhist_hypo_1e1p(  hname_hypo_1e1p.str().c_str(),"",32,0,32);

    //vic
    flashhist_hypo_1e1p.SetDirectory(_fout);
    flashhist_hypo_1mu1p.SetDirectory(_fout);

    float maxpe_hypo_1e1p = 0.;
    float maxpe_hypo_1mu1p = 0.;    
    float petot_hypo_1e1p = 0.;
    float petot_hypo_1mu1p = 0.;
    for (int i=0; i<32; i++){
      // 1mu1p
      flashhist_hypo_1mu1p.SetBinContent(i+1, hypo_1mu1p.pe_v[i] );
      if ( maxpe_hypo_1mu1p<hypo_1mu1p.pe_v[i] )
	maxpe_hypo_1mu1p = hypo_1mu1p.pe_v[i];
      petot_hypo_1mu1p += hypo_1mu1p.pe_v[i];

      // 1e1p
      flashhist_hypo_1e1p.SetBinContent(i+1, hypo_1e1p.pe_v[i] );
      if ( maxpe_hypo_1e1p<hypo_1e1p.pe_v[i] )
	maxpe_hypo_1e1p = hypo_1e1p.pe_v[i];
      petot_hypo_1e1p += hypo_1e1p.pe_v[i];

    }// num of pmts

    maxpe_hypo_1e1p /= petot_hypo_1e1p;
    maxpe_hypo_1mu1p /= petot_hypo_1mu1p;

    
    flashhist_hypo_1e1p.Scale(1.0/petot_hypo_1e1p);
    flashhist_hypo_1e1p.SetLineColor(kRed);
    
    flashhist_hypo_1mu1p.Scale(1.0/petot_hypo_1mu1p);
    flashhist_hypo_1mu1p.SetLineColor(kRed);    

    
    std::vector<TH1D*> flashhist_data_v;
    std::vector<float> petot_data_v;    
    float maxpe_data = 0.;
    
    for ( size_t i=0; i<ev_opflash.size(); i++) {
      std::stringstream hname_data;
      hname_data << "hflash_" << run << "_" << event << "_" << subrun << "_flash" << i << "_data";
      TH1D* flashhist_data = new TH1D( hname_data.str().c_str(),"",32,0,32);
      flashhist_data->SetDirectory(_fout);
      float datatot = 0.;
      for (int ipmt=0; ipmt<32; ipmt++) {
	flashhist_data->SetBinContent(ipmt+1,dataflash_v[i].pe_v[ipmt]);
	datatot += dataflash_v[i].pe_v[ipmt];
	if ( dataflash_v[i].pe_v[ipmt]>maxpe_data )
	  maxpe_data = dataflash_v[i].pe_v[ipmt];
	flashhist_data->SetLineColor(kBlack);
      }
      flashhist_data->Scale( 1.0/datatot );
      flashhist_data_v.push_back( flashhist_data );
      maxpe_data /= datatot;
      petot_data_v.push_back( datatot );
      flashhist_data->Write();
      delete flashhist_data;
    }// end of flashes

    
    // calculate simple chi2
    /*
    best_chi2 = -1;
    best_data_totpe = -1;
    int best_data = 0;
    int idata = -1;
    for ( auto& phist : flashhist_data_v ) {
      idata++;
      float chi2 = 0.;
      for (int ipmt=0; ipmt<32; ipmt++) {
	float pefrac_data = phist->GetBinContent(ipmt+1);
	float pefrac_hypo = flashhist_hypo.GetBinContent(ipmt+1);

	float pe_data = pefrac_data*petot_data_v[idata];
	float pefrac_data_err = sqrt(pe_data)/petot_data_v[idata];

	float diff = pefrac_hypo - pefrac_data;
	
	LLCV_DEBUG() << "[" << ipmt << "] diff=" << diff << " hypo=" << pefrac_hypo << " data=" << pefrac_data << std::endl;
	
	// i know this is all fubar
	if ( pefrac_data_err>0 )
	  chi2 += (diff*diff)/pefrac_data;
	else if (pefrac_data_err==0.0 && pefrac_hypo>0){
	  chi2 += (diff*diff)/pefrac_hypo;
	}
	
      }
      if ( best_chi2<0 || best_chi2>chi2 ) {
	best_chi2 = chi2;
	best_data_totpe = petot_data_v[idata];
	best_data = idata;
      }
    }


    hypo_totpe = 0.0;
    
    for (int ipmt=0; ipmt<32; ipmt++) {
      data_pe[ipmt] = 0.0;

      for(auto& phist : flashhist_data_v)
	data_pe[ipmt] += phist->GetBinContent(ipmt+1);

      hypo_pe[ipmt]       = flashhist_hypo.GetBinContent(ipmt+1);
      hypo_protonpe[ipmt] = flashhist_hypo_proton.GetBinContent(ipmt+1);
      hypo_showerpe[ipmt] = flashhist_hypo_shower.GetBinContent(ipmt+1);
      hypo_totpe += hypo_pe[ipmt];
    }

    for ( auto& phist : flashhist_data_v )
      delete phist;

    flashhist_data_v.clear();
    */

    flashhist_hypo_1mu1p.Write();
    flashhist_hypo_1e1p.Write();

    outtree->Fill();
    
    LLCV_DEBUG() << "end" << std::endl;
    return 1;
  }

  flashana::QCluster_t InterSelFlashMatch::build1mu1pQCluster( const int protonid, std::vector<larlitecv::TrackHitSorter>& dedxgen_v ) {
    // we use the proton id to build proton hypothesis
    // other tracks are assumed as muons

    const float ly_proton   = 19200;
    const float ly_muon     = 24000;
    
    flashana::QCluster_t qinteraction; // should reserve
    qinteraction.reserve( 1000 );
    for ( size_t itrack=0; itrack<dedxgen_v.size(); itrack++ ) {
      // we get the dedx track
      std::vector< std::vector<float> > dedx_track_per_plane;
      dedxgen_v[itrack].getPathBinneddEdx( 0.5, 0.5, dedx_track_per_plane );
      const std::vector< std::vector<float> >& bincenter_xyz = dedxgen_v[itrack].getBinCentersXYZ( 2 ); // todo: use v plane if y plane missing too many pieces

      float ly = ly_muon;
      if ( (int)itrack==protonid )
	ly = ly_proton;

      for (int ipt=0; ipt<(int)dedx_track_per_plane[2].size(); ipt++) {
	float dedx = dedx_track_per_plane[2].at(ipt);
	const std::vector<float>& edep_pos = bincenter_xyz[ipt];
	float numphotons = dedx*(2*0.5)*ly;
	flashana::QPoint_t qpt( edep_pos[0], edep_pos[1], edep_pos[2], numphotons );
	qinteraction.emplace_back( std::move(qpt) );
      }
      // we get the 
    }
    return qinteraction;
  }

  flashana::QCluster_t InterSelFlashMatch::build1e1pQCluster( const int protonid, const int shrid,
							      const larlite::vertex& vtx, const larlite::shower& shreco, 
							      std::vector<larlitecv::TrackHitSorter>& dedxgen_v ) {
    // we use the proton id to build proton hypothesis. same with shrid.
    // any leftover tracks are assumed as muons

    const float ly_proton   = 19200;
    const float ly_muon     = 24000;
    const float ly_shower   = 20000;
    const float shower_correction_factor = 50;
    
    flashana::QCluster_t qinteraction; // should reserve
    qinteraction.reserve( 1000 );
    for ( size_t itrack=0; itrack<dedxgen_v.size(); itrack++ ) {

      if ( (int)itrack!=shrid ) {
	// we get the dedx track
	std::vector< std::vector<float> > dedx_track_per_plane;
	dedxgen_v[itrack].getPathBinneddEdx( 0.5, 0.5, dedx_track_per_plane );
	const std::vector< std::vector<float> >& bincenter_xyz = dedxgen_v[itrack].getBinCentersXYZ( 2 ); // todo: use v plane if y plane missing too many pieces
	
	float ly = ly_muon;
	if ( (int)itrack==protonid )
	  ly = ly_proton;

	for (int ipt=0; ipt<(int)dedx_track_per_plane[2].size(); ipt++) {
	  float dedx = dedx_track_per_plane[2].at(ipt);
	  const std::vector<float>& edep_pos = bincenter_xyz[ipt];
	  float numphotons = dedx*(2*0.5)*ly;
	  flashana::QPoint_t qpt( edep_pos[0], edep_pos[1], edep_pos[2], numphotons );
	  qinteraction.emplace_back( std::move(qpt) );
	}
      }//end of if not-shower
      else {
	// make shower qcluser: each point corresponds to the number of photons
	float shower_energy = Tree().Scalar<float>("reco_LL_electron_energy");
	float used_energy = 2.0*shower_energy; // correction factor for under-clustering (HACK)
	float used_length = used_energy/2.4; // MeV / (MeV/cm)

	LLCV_DEBUG() << " shower: used energy=" << used_energy << " used length=" << used_length << std::endl;

	float maxstepsize = 0.3;
	int nsteps = used_length/maxstepsize+1;
	float step = used_length/float(nsteps);
	for (int istep=0; istep<=nsteps; istep++) {
	  double pos[3];
	  pos[0] = vtx.X() + (step*istep)*shreco.Direction().X();
	  pos[1] = vtx.Y() + (step*istep)*shreco.Direction().Y();
	  pos[2] = vtx.Z() + (step*istep)*shreco.Direction().Z();      
	  float numphotons = (shower_correction_factor*step)*ly_shower;
	  flashana::QPoint_t qpt( pos[0], pos[1], pos[2], numphotons );
	  qinteraction.push_back( qpt );
	}
      }//end of if shower cluster
    }//end of track loop
    
    return qinteraction;
  }
  
  void InterSelFlashMatch::Finalize() {
    LLCV_DEBUG() << "start" << std::endl;

    _fout->cd();
    outtree->Write();

    LLCV_DEBUG() << "end" << std::endl;
  }

}


#endif
