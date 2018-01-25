#ifndef __INTERSELBASE_H__
#define __INTERSELBASE_H__

//llcv
#include "LLCVBase/llcv_base.h"
#include "InterTTreeManager.h"
#include "InterImageManager.h"
#include "InterDataManager.h"

//larcv
#include "Base/PSet.h"

//ROOT
#include "TFile.h"

namespace llcv {

  class InterDriver;  

  class InterSelBase : public llcv_base { 

    friend class InterDriver;

  public:

  InterSelBase(std::string name="InterSelBase") : llcv_base(name), _name(name) {}
    virtual ~InterSelBase(){}
    
    virtual void Configure (const larcv::PSet &pset) = 0;
    virtual void Initialize() = 0;
    virtual double Select() = 0;
    virtual void Finalize() = 0;

    
  protected:
    std::string _name;

    TFile* _fout;

    InterImageManager& Image()      { return *_img_mgr_ptr; }
    const InterDataManager& Data()  { return *_data_mgr_ptr; }
    const InterTTreeManager& Tree() { return *_tree_mgr_ptr; }

  private:

    void set_output_file(TFile* fout) { _fout = fout; }
    
    InterImageManager* _img_mgr_ptr;
    const InterDataManager* _data_mgr_ptr;
    const InterTTreeManager* _tree_mgr_ptr;


    
  };

}


#endif
