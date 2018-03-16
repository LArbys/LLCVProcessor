#ifndef __INTERPROTONID_H__
#define __INTERPROTONID_H__

#include "InterTool_Core/InterSelBase.h"
#include "FlashMatchInterface/GeneralFlashMatchAlgo.h"
#include "TrackHitSorter/TrackHitSorter.h"

// larlite
#include "DataFormat/vertex.h"
#include "DataFormat/track.h"

namespace llcv {
  
  class InterProtonID : public InterSelBase { 

  public:
    
  InterProtonID(std::string name="InterProtonID") : 
    InterSelBase(name)
      ,outtree(nullptr)
    {}

    ~InterProtonID(){}
    
    void Configure (const larcv::PSet &pset);
    void Initialize();
    double Select();
    void Finalize();
    
    
  private:

    // -----------------------------
    // recorded trees
    TTree* outtree;


    // note: pmt values are opchannel-indexed
    
    float vtxpos_x;
    float vtxpos_y;
    float vtxpos_z;
    
    int number_tracks;
    int number_showers;

    std::vector< std::vector<float> > m_track_dedx_vv;
    std::vector< std::vector<float> > m_track_bincenters_vv[3];

    void ResetVertex();

    // -----------------------------
    // running parameters
    float fmax_hit_radius_cm;
    float fbinwidth_cm;
    float fbinstride_cm;
    bool  fSavedEdxGraphs;
    bool  fIsMC;


  };

}


#endif
