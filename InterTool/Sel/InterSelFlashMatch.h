#ifndef __INTERSELFLASHMATCH_H__
#define __INTERSELFLASHMATCH_H__

#include "InterTool_Core/InterSelBase.h"
#include "FlashMatchInterface/GeneralFlashMatchAlgo.h"

namespace llcv {
  
  class InterSelFlashMatch : public InterSelBase { 

  public:
    
  InterSelFlashMatch(std::string name="InterSelFlashMatch") : 
    InterSelBase(name)
      ,outfile(nullptr)
      ,outtree(nullptr)
      ,genflashmatch(nullptr) 
    {}

    ~InterSelFlashMatch(){}
    
    void Configure (const larcv::PSet &pset);
    double Select();
    void Finalize();
    
    
  private:
    
    TFile* outfile;
    TTree* outtree;
    float best_chi2;
    float best_data_totpe;
    float hypo_totpe;
    float hypo_pe[32];
    float hypo_protonpe[32];
    float hypo_showerpe[32];  
    float data_pe[32];
    float vtxpos[3];

    larlitecv::GeneralFlashMatchAlgo* genflashmatch;
    float shower_correction_factor;    
    
  };

}


#endif
