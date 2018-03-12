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
    
    TTree* outtree;

    float best_chi2;
    float best_data_totpe;
    float hypo_totpe;
    float hypo_pe[32];
    float hypo_protonpe[32];
    float hypo_showerpe[32];  
    float data_pe[32];
    float vtxpos[3];

     float reco_nu_E;
     float reco_shower_E;
     float reco_proton_E;

     float true_nu_E;
     float true_shower_E;


     float true_proton_E;

     int valid;
     float scedr;

    larlitecv::GeneralFlashMatchAlgo* genflashmatch;
    float shower_correction_factor;
    float fmax_hit_radius;
    bool isMC;

    flashana::QCluster_t build1mu1pQCluster( const int protonid, std::vector<larlitecv::TrackHitSorter>& dedxgen_v );
    flashana::QCluster_t build1e1pQCluster( const int protonid, const int shrid,
					    const larlite::vertex& vtx, const larlite::shower& shreco,
					    std::vector<larlitecv::TrackHitSorter>& dedxgen_v );
    
  };

}


#endif
