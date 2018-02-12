#ifndef __SELCOSMICID_CXX__
#define __SELCOSMICID_CXX__

#include "SelCosmicID.h"

namespace llcv {

  void SelCosmicID::Configure (const larcv::PSet &pset) {
    set_verbosity((msg::Level_t)pset.get<int>("Verbosity",2));
    LLCV_DEBUG() << "start" << std::endl;

    LLCV_DEBUG() << "end" << std::endl;
  }

  void SelCosmicID::Initialize() {
    _fout->cd();
    _outtree = new TTree("CosmicID","");

    AttachRSEV(_outtree);
    
    return;
  }

  double SelCosmicID::Select() {
    LLCV_DEBUG() << "start" << std::endl;
    LLCV_DEBUG() << "=======================" << std::endl;
    
    
    //
    // get the image centered around the vertex
    //
    
    auto mat_v = Image().Image<cv::Mat>(kImageADC,20,20);
 
    LLCV_DEBUG() << "(RSEV)=("<< Run() << "," << SubRun() << "," << Event() << "," << VertexID() << ")" << std::endl;
    
    _outtree->Fill();
    
    LLCV_DEBUG() << "=======================" << std::endl;
    LLCV_DEBUG() << "end" << std::endl;
    return 0.0;
  }
  
  void SelCosmicID::Finalize() {
    LLCV_DEBUG() << "start" << std::endl;

    _fout->cd();
    _outtree->Write();

    LLCV_DEBUG() << "end" << std::endl;
  }

}


#endif
