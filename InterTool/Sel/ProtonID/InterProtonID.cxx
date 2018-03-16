#ifndef __INTERPROTONID_CXX__
#define __INTERPROTONID_CXX__

#include "InterProtonID.h"

#include "TCanvas.h"
#include "TH1D.h"

#include <stdlib.h>

namespace llcv {

  void InterProtonID::Configure (const larcv::PSet &pset) {
    
    set_verbosity((msg::Level_t)pset.get<int>("Verbosity",2));
    LLCV_DEBUG() << "start" << std::endl;
    fmax_hit_radius_cm = pset.get<float>("MaxHitRadiuscm");
    fbinwidth_cm       = pset.get<float>("BinWidthcm");
    fbinstride_cm      = pset.get<float>("BinStridecm");
    fSavedEdxGraphs    = pset.get<bool>("SavedEdxGraphs",false); // for debugging
    fIsMC              = pset.get<bool>("IsMC",false); // for truth-study quantities
    
    LLCV_DEBUG() << "end" << std::endl;
  }
  
  void InterProtonID::Initialize() {
    if(!_fout) throw llcv_err("No output file?");

    _fout->cd();

    outtree = new TTree("protonid","");
    AttachRSEV(outtree);

    // rsev
    outtree->Branch("vtxpos_x",&vtxpos_x,"vtxpos_x/F");
    outtree->Branch("vtxpos_y",&vtxpos_y,"vtxpos_y/F");
    outtree->Branch("vtxpos_z",&vtxpos_z,"vtxpos_z/F");

    outtree->Branch("number_tracks",&number_tracks,"number_tracks/I");
    outtree->Branch("number_showers",&number_showers,"number_showers/I");
    
    
  }

  double InterProtonID::Select() {
    LLCV_DEBUG() << "start" << std::endl;
    
    ResetVertex();
    
    const larlite::vertex& vtx = *(Data().Vertex());

    vtxpos_x = (float)vtx.X();
    vtxpos_y = (float)vtx.Y();
    vtxpos_z = (float)vtx.Z();

    // GET DATA
    // --------
    // get hits
    const auto& phit_v = Data().Hits();

    std::vector< larlite::hit > hit_v;
    hit_v.reserve(phit_v.size());
    for (auto const& phit : phit_v ) hit_v.push_back(*phit);
    
    // // get prong class
    // shrid = 0;
    // protonid = -1;    
    // shrid = Tree().Scalar<int>("shrid");
    // if ( shrid==0 )
    //   protonid = 1;
    // else if (shrid==1)
    //   protonid = 0;

    std::vector<int> hitmask_v(hit_v.size(),1);
    LLCV_DEBUG() << "number of hits: " << hit_v.size() << std::endl;
    
    LLCV_DEBUG() << "Number of showers: " << Data().Showers().size() << std::endl;
    LLCV_DEBUG() << "Number of tracks: "  << Data().Tracks().size() << std::endl;    

    // get shower
    const auto& shr_v = Data().Showers();

    // get tracks
    const auto& trk_v = Data().Tracks();

    number_tracks = (int)trk_v.size();
    number_showers= (int)shr_v.size();

    // this class will generated de/dx per 3d position along the track. will use to set MeV deposited at 3d pos.
    std::vector<larlitecv::TrackHitSorter> dedxgen_v(trk_v.size());

    // ok, we now have dedx 3d tracks for all tracks and shower
    // we want to build 1mu1p and 1e1p hypothesis


    outtree->Fill();
    
    LLCV_DEBUG() << "end" << std::endl;
    return 1;
  }


  void InterProtonID::ResetVertex() {
    
    vtxpos_x = kINVALID_FLOAT;
    vtxpos_y = kINVALID_FLOAT;
    vtxpos_z = kINVALID_FLOAT;

    number_tracks = -1*kINVALID_INT;
    number_showers= -1*kINVALID_INT;
    

  }

  void InterProtonID::Finalize() {
    LLCV_DEBUG() << "start finalize" << std::endl;
    _fout->cd();
    outtree->Write();
    LLCV_DEBUG() << "end finalize" << std::endl;
  }

}


#endif
