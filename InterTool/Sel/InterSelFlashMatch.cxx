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

  double InterSelFlashMatch::Select() {
    LLCV_DEBUG() << "start" << std::endl;
    
    if(!outfile) {
      outfile = new TFile("aho.root","RECREATE");
      outfile->cd();
      outtree = new TTree("ffmatch","Final Flash Match result");
      outtree->SetDirectory(outfile);
      outtree->Branch("vtxpos",vtxpos,"vtxpos[3]/F");
      outtree->Branch("chi2",&best_chi2,"chi2/F");
      outtree->Branch("data_totpe",&best_data_totpe,"data_totpe/F");
      outtree->Branch("hypo_totpe",&hypo_totpe,"hypo_totpe/F");
      outtree->Branch("hypo_pe",hypo_pe,"hypo_pe[32]/F");
      outtree->Branch("data_pe",data_pe,"data_pe[32]/F");
      outtree->Branch("hypo_protonpe",hypo_protonpe,"hypo_protonpe[32]/F");
      outtree->Branch("hypo_showerpe",hypo_showerpe,"hypo_showerpe[32]/F");
    }

    // only do this for close vertex
    if (Tree().Scalar<float>("locv_scedr") < 5.0) {
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
    // https://raw.githubusercontent.com/LArbys/finalflashmatch/master/finalflashmatch/run_final_flashmatch.cxx
    //
    // thanks taritree


    const auto& vtx = *(Data().Vertex());

    std::cout << "  vertex: (" << vtx.X() << "," << vtx.Y() << "," << vtx.Z() << ")" << std::endl;

    // Light Yields
    // proton: 19200
    // electron: 20000
    // muon: 24000
    
    // make the proton qcluster
    flashana::QCluster_t qinteraction;
    flashana::QCluster_t qshower;
    flashana::QCluster_t qproton;

    // get energy

    //vic
    // std::cout << "  proton energy: ";
    // for ( auto& protonenergy : *pproton_energy ) {
    //   std::cout << " " << protonenergy;
    // }
    // std::cout << std::endl;
    //vic

    // get positions 
    float maxstepsize = 0.3;
    
    //vic
    //const larlite::track& lltrack = ev_track->front();
    const larlite::track& lltrack = *(Data().Tracks().front());
    //vic
    std::cout << "  track (shreco): "
	      << " npts=" << lltrack.NumberTrajectoryPoints()
	      << " start=(" << lltrack.Vertex().X() << "," << lltrack.Vertex().Y() << "," << lltrack.Vertex().Z() << ")"
	      << " end=(" << lltrack.End().X() << "," << lltrack.End().Y() << "," << lltrack.End().Z() << ")"      
	      << std::endl;

    
    // proton (from tracker)
    //vic
    // std::cout << "  proton "
    // 	      << " start=(" << proton_startpt[0] << "," << proton_startpt[1] << "," << proton_startpt[2] << ") -> "
    // 	      << " end=(" << proton_endpt[0] << "," << proton_endpt[1] << "," << proton_endpt[2] << ")"
    // 	      << std::endl;
    // float pdir[3] = {0};
    // float plen = 0.;
    // for (int i=0; i<3; i++) {
    //   pdir[i] = proton_endpt[i]-proton_startpt[i];
    //   plen += pdir[i]*pdir[i];
    // }
    // plen = sqrt(plen);
    // for (int i=0; i<3; i++)
    //   pdir[i] /= plen;
    //vic


    // get energy from range
    //vic 
    // double proton_energy_fromrange = astracker_algo.GetEnergy("proton",plen); // MeV
    // double proton_dedx = proton_energy_fromrange/plen;

    // std::cout << "  proton energy from range: " << proton_energy_fromrange << " MeV"
    // 	      << " length=" << plen << " cm"
    // 	      << " de/dx=" << proton_dedx
    // 	      << std::endl;
    //vic
    
    // int nsteps = plen/maxstepsize+1;
    // float step = plen/float(nsteps);
    int nsteps;
    float step;
    // for (int istep=0; istep<=nsteps; istep++) {
    //   float pos[3];
    //   for (int i=0; i<3; i++)
    // pos[i] = proton_startpt[i] + (step*istep)*pdir[i];
    //   float numphotons = proton_dedx*step*19200;
    //   flashana::QPoint_t qpt( pos[0], pos[1], pos[2], numphotons );
    //   qinteraction.push_back( qpt );
    //   qproton.push_back( qpt );
    // }
    // proton at single point for now
    
    //vic
    //float numphotons_proton = 19200.0*(*pproton_energy).front();
    float numphotons_proton = 19200.0 * pproton_energy;
    //vic
    flashana::QPoint_t qptproton( vtx.X(), vtx.Y(), vtx.Z(), numphotons_proton );
    qproton.push_back( qptproton );
    std::cout << "  proton: numphotons=" << numphotons_proton << std::endl;
    
    
    // make shower qcluser: each point corresponds to the number of photons
    //vic
    // const larlite::shower& shreco = ev_shreco->front();
    const larlite::shower& shreco = *(Data().Showers().front());
    //vic
    std::cout << "  shower (larlite reco): "
	      << " dir=(" << shreco.Direction().X() << "," << shreco.Direction().Y() << "," << shreco.Direction().Z() << ")"
	      << " energy=(" << shreco.Energy(0) << "," << shreco.Energy(1) << "," << shreco.Energy(2) << ")"
	      << " energy=" << shreco.Energy()
	      << " length=" << shreco.Length()
	      << std::endl;
    //vic
    // if ( hasMC ) {
    //   std::cout << "  shower (qual): "
    // 		<< " dir2=(" << shower_dir[0] << "," << shower_dir[1] << "," << shower_dir[2] << ") "
    // 		<< " energy2=(" << shower_energy[0] << "," << shower_energy[1] << "," << shower_energy[2] << ")"
    // 		<< " shower_len=" << shower_len
    // 		<< " mclength=" << shower_mclen
    // 		<< " mcenergy=" << shower_mcenergy
    // 		<< std::endl;      
    // }
    //vic
    
    //vic
    //float used_energy = 2.0*shreco.Energy();
    float used_energy = 2.0*Tree().Scalar<float>("reco_LL_electron_energy");
    //vic
    float used_length = used_energy/2.4; // MeV / (MeV/cm)

    std::cout << " shower: used energy=" << used_energy << " used length=" << used_length << std::endl;
    
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


    // convert opflash into flash_t

    //vic
    const auto& opflash_ptr_v = Data().Flashes();
    std::vector<larlite::opflash> ev_opflash;
    ev_opflash.reserve(opflash_ptr_v.size());
    for(const auto& opflash_ptr : opflash_ptr_v)
      ev_opflash.emplace_back(*opflash_ptr);
    //vic

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
    flashhist_hypo.SetDirectory(outfile);
    flashhist_hypo_proton.SetDirectory(outfile);
    flashhist_hypo_shower.SetDirectory(outfile);
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
    
    //vic
    //for ( size_t i=0; i<ev_opflash->size(); i++) {
    for ( size_t i=0; i<ev_opflash.size(); i++) {
      //vic
      std::stringstream hname_data;
      hname_data << "hflash_" << run << "_" << event << "_" << subrun << "_data" << i;
      TH1D* flashhist_data = new TH1D( hname_data.str().c_str(),"",32,0,32);
      flashhist_data->SetDirectory(outfile);
      float datatot = 0.;
      for (int ipmt=0; ipmt<32; ipmt++) {
	flashhist_data->SetBinContent(ipmt+1,dataflash_v[i].pe_v[ipmt]);
	datatot += dataflash_v[i].pe_v[ipmt];
	//std::cout << "data [" << ipmt << "] " << dataflash_v[i].pe_v[ipmt] << std::endl;
	if ( dataflash_v[i].pe_v[ipmt]>maxpe_data )
	  maxpe_data = dataflash_v[i].pe_v[ipmt];
	flashhist_data->SetLineColor(kBlack);
      }
      flashhist_data->Scale( 1.0/datatot );
      flashhist_data_v.push_back( flashhist_data );
      maxpe_data /= datatot;
      petot_data_v.push_back( datatot );
    }// end of flashes

    //vic
    bool makePNGs = false;
    //vic
    if ( makePNGs ) {
      TCanvas cflash(hname_hypo.str().c_str(),"",800,600);
      if ( maxpe_hypo<maxpe_data )
	flashhist_hypo.GetYaxis()->SetRangeUser(0,maxpe_data*1.1);
      flashhist_hypo.Draw("hist");
      flashhist_hypo_proton.Draw("histsame");
      flashhist_hypo_shower.Draw("histsame");
      for ( auto const& phist : flashhist_data_v ) {
	phist->Draw("same");
      }
      hname_hypo << ".png";
      
      cflash.SaveAs( hname_hypo.str().c_str() );
    }


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
	
	std::cout << "[" << ipmt << "] diff=" << diff << " hypo=" << pefrac_hypo << " data=" << pefrac_data << std::endl;
	
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

    for ( auto& phist : flashhist_data_v ) {
    outfile->cd();
      phist->Write();
      delete phist;
    }

    outfile->cd();
    flashhist_hypo.Write();
    outfile->cd();
    flashhist_hypo_proton.Write();
    outfile->cd();
    flashhist_hypo_shower.Write();

    outtree->Fill();
    
    LLCV_DEBUG() << "end" << std::endl;
    return 1;
  }
  
  void InterSelFlashMatch::Finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    std::cout << outfile << std::endl;
    outfile->cd();
    //outtree->Write();
    outtree->GetCurrentFile()->Write();
    outfile->Close();
    LLCV_DEBUG() << "end" << std::endl;
  }

}


#endif
