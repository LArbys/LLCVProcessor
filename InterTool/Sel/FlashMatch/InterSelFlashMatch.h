#ifndef __INTERSELFLASHMATCH_H__
#define __INTERSELFLASHMATCH_H__

#include "InterTool_Core/InterSelBase.h"
#include "FlashMatchInterface/GeneralFlashMatchAlgo.h"
#include "TrackHitSorter/TrackHitSorter.h"

// larlite
#include "DataFormat/vertex.h"
#include "DataFormat/track.h"

namespace llcv {
  
  class InterSelFlashMatch : public InterSelBase { 

  public:
    
  InterSelFlashMatch(std::string name="InterSelFlashMatch") : 
    InterSelBase(name)
      ,outtree(nullptr)
      ,genflashmatch(nullptr) 
    {}

    ~InterSelFlashMatch(){}
    
    void Configure (const larcv::PSet &pset);
    void Initialize();
    double Select();
    void Finalize();
    
    
  private:

    // -----------------------------
    // recorded trees
    TTree* outtree;


    // note: pmt values are opchannel-indexed
    
    // data flash
    float data_pe[32];
    float data_totpe;
    int ndata_flashes;
    int data_flashidx;
    
    // 1mu1p
    float best_chi2_1mu1p;   
    float hypo_totpe_1mu1p;  // for global ly constraint
    float hypo_pe_1mu1p[32]; // for pmt-pmt correction

    // 1e1p
    float best_chi2_1e1p;
    float hypo_totpe_1e1p;  // for global ly constraint
    float hypo_pe_1e1p[32]; // for pmt-pmt correction

    int valid;
     
    int run;
    int subrun;
    int event;
    int vertexid;
    float vtxpos[3];
    int shrid;
    int protonid;
    float longestdir[3];

    // from inter file
    float reco_nu_E;
    float reco_shower_E;
    float reco_proton_E;

    // from MC
    float true_nu_E;
    float true_shower_E;
    float true_proton_E;
    float scedr;

    // -----------------------------
    // running parameters
    larlitecv::GeneralFlashMatchAlgo* genflashmatch;
    float shower_correction_factor;
    float fmax_hit_radius;
    bool  isMC;
    bool  fSaveHistograms;

    flashana::QCluster_t build1mu1pQCluster( const int protonid, std::vector<larlitecv::TrackHitSorter>& dedxgen_v );
    flashana::QCluster_t build1e1pQCluster( const int protonid, const int shrid,
					    const larlite::vertex& vtx, const larlite::shower& shreco,
					    std::vector<larlitecv::TrackHitSorter>& dedxgen_v );

    //float calcShapeOnlyChi2( const flashana::
    
  };

}


#endif
