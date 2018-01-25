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
    
    LLCV_DEBUG() << "end" << std::endl;
  }

  void InterSelFlashMatch::Initialize() {
    if(!_fout) throw llcv_err("No output file?");

    _fout->cd();

    outtree = new TTree("ffmatch","Final Flash Match result");
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
  }


  double InterSelFlashMatch::Select() {
    LLCV_DEBUG() << "start" << std::endl;
    

    // only do this for close vertex
    if (Tree().Scalar<float>("locv_scedr") > 5.0) {
      LLCV_INFO() << "Poor reconstruction" << std::endl;
      return 0;
    }
    
    // get the reconstructed proton energy (from selection)
    auto pproton_energy = Tree().Scalar<float>("reco_LL_proton_energy");
    if (pproton_energy < 0)  {
      LLCV_INFO() << "No reconstructed proton energy" << std::endl;
      return 0;
    }


    // 
    // adapated from taritree's code:
    // https://goo.gl/8MboZe
    //


    const auto& vtx = *(Data().Vertex());

    vtxpos[0] = (float)vtx.X();
    vtxpos[1] = (float)vtx.Y();
    vtxpos[2] = (float)vtx.Z();


    reco_nu_E     = Tree().Scalar<float>("reco_LL_total_energy");
    reco_shower_E = Tree().Scalar<float>("reco_LL_electron_energy");
    reco_proton_E = Tree().Scalar<float>("reco_LL_proton_energy");
    
    true_nu_E     = Tree().Scalar<float>("locv_energyInit");
    true_shower_E = Tree().Scalar<float>("anashr1_mc_energy");
    true_proton_E = Tree().Scalar<float>("locv_dep_sum_proton");
    
    // Light Yields
    // proton: 19200
    // electron: 20000
    // muon: 24000
    
    flashana::QCluster_t qinteraction;
    flashana::QCluster_t qshower;
    flashana::QCluster_t qproton;

    // get positions 
    auto shrid = Tree().Scalar<int>("reco_LL_electron_id");
    auto trkid = Tree().Scalar<int>("reco_LL_proton_id");

    const larlite::track& lltrack = *(Data().Tracks().at(trkid));
    const larlite::shower& shreco = *(Data().Showers().at(shrid));

    int nsteps;
    float step; 

    // make proton qcluster

    float numphotons_proton = 19200.0 * pproton_energy;
    flashana::QPoint_t qptproton( vtx.X(), vtx.Y(), vtx.Z(), numphotons_proton );
    qproton.push_back( qptproton );
    LLCV_DEBUG() << "  proton: numphotons=" << numphotons_proton << std::endl;
    
    
    // make shower qcluser: each point corresponds to the number of photons
    float shower_energy = Tree().Scalar<float>("reco_LL_electron_energy");
    float used_energy = 2.0*shower_energy; // correction factor for under-clustering
    float used_length = used_energy/2.4; // MeV / (MeV/cm)

    LLCV_DEBUG() << " shower: used energy=" << used_energy << " used length=" << used_length << std::endl;

    float maxstepsize = 0.3;
    nsteps = used_length/maxstepsize+1;
    step = used_length/float(nsteps);
    for (int istep=0; istep<=nsteps; istep++) {
      double pos[3];
      pos[0] = vtx.X() + (step*istep)*shreco.Direction().X();
      pos[1] = vtx.Y() + (step*istep)*shreco.Direction().Y();
      pos[2] = vtx.Z() + (step*istep)*shreco.Direction().Z();      
      float numphotons = (shower_correction_factor*step)*20000;
      flashana::QPoint_t qpt( pos[0], pos[1], pos[2], numphotons );
      qinteraction.push_back( qpt );
      qshower.push_back( qpt );
    }


    const auto& opflash_ptr_v = Data().Flashes();
    std::vector<larlite::opflash> ev_opflash;
    ev_opflash.reserve(opflash_ptr_v.size());
    for(const auto& opflash_ptr : opflash_ptr_v)
      ev_opflash.emplace_back(*opflash_ptr);

    std::vector<flashana::Flash_t> dataflash_v = genflashmatch->MakeDataFlashes( ev_opflash );
    
    // make flash hypothesis
    flashana::Flash_t hypo        = genflashmatch->GenerateUnfittedFlashHypothesis( qinteraction );
    flashana::Flash_t hypo_proton = genflashmatch->GenerateUnfittedFlashHypothesis( qproton );
    flashana::Flash_t hypo_shower = genflashmatch->GenerateUnfittedFlashHypothesis( qshower );    

    // make flash hist
    std::stringstream hname_hypo;

    auto run    = Tree().Scalar<int>("run");
    auto subrun = Tree().Scalar<int>("subrun");
    auto event  = Tree().Scalar<int>("event");

    hname_hypo << "hflash_" << run << "_" << event << "_" << subrun << "_hypo";
    std::stringstream hname_hypo_proton;
    hname_hypo_proton << "hflash_" << run << "_" << event << "_" << subrun << "_hypoproton";
    std::stringstream hname_hypo_shower;
    hname_hypo_shower << "hflash_" << run << "_" << event << "_" << subrun << "_hyposhower";
    
    TH1D flashhist_hypo( hname_hypo.str().c_str(),"",32,0,32);
    TH1D flashhist_hypo_proton( hname_hypo_proton.str().c_str(),"",32,0,32);
    TH1D flashhist_hypo_shower( hname_hypo_shower.str().c_str(),"",32,0,32);        

    //vic
    flashhist_hypo.SetDirectory(_fout);
    flashhist_hypo_proton.SetDirectory(_fout);
    flashhist_hypo_shower.SetDirectory(_fout);
    //vic

    float maxpe_hypo = 0.;
    float petot_hypo = 0.;
    for (int i=0; i<32; i++){
      flashhist_hypo.SetBinContent( i+1, hypo.pe_v[i] );
      flashhist_hypo_proton.SetBinContent(i+1, hypo_proton.pe_v[i] );
      flashhist_hypo_shower.SetBinContent(i+1, hypo_shower.pe_v[i] );      
      if ( maxpe_hypo<hypo.pe_v[i] )
	maxpe_hypo = hypo.pe_v[i];
      petot_hypo += hypo.pe_v[i];
    }// num of pmts

    maxpe_hypo /= petot_hypo;
    flashhist_hypo.Scale(1.0/petot_hypo);
    flashhist_hypo_proton.Scale(1.0/petot_hypo);
    flashhist_hypo_shower.Scale(1.0/petot_hypo);    
    flashhist_hypo.SetLineColor(kRed);
    flashhist_hypo_proton.SetLineColor(kCyan);
    flashhist_hypo_shower.SetLineColor(kMagenta);    
    
    std::vector<TH1D*> flashhist_data_v;
    std::vector<float> petot_data_v;    
    float maxpe_data = 0.;
    
    for ( size_t i=0; i<ev_opflash.size(); i++) {
      std::stringstream hname_data;
      hname_data << "hflash_" << run << "_" << event << "_" << subrun << "_data" << i;
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
    }// end of flashes

    // calculate simple chi2
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

    outtree->Fill();
    
    LLCV_DEBUG() << "end" << std::endl;
    return 1;
  }
  
  void InterSelFlashMatch::Finalize() {
    LLCV_DEBUG() << "start" << std::endl;

    _fout->cd();
    outtree->Write();

    LLCV_DEBUG() << "end" << std::endl;
  }

}


#endif
