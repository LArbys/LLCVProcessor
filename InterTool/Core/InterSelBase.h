#ifndef __INTERSELBASE_H__
#define __INTERSELBASE_H__

//llcv
#include "LLCVBase/llcv_base.h"
#include "InterTTreeManager.h"
#include "InterImageManager.h"
#include "InterDataManager.h"

//larcv
#include "Base/PSet.h"

namespace llcv {
  
  class InterSelBase : public llcv_base { 

  public:

  InterSelBase(std::string name="InterSelBase") : llcv_base(name), _name(name) {}
    virtual ~InterSelBase(){}
    
    virtual void Configure (const larcv::PSet &pset) = 0;
    virtual float Select() = 0;
    virtual void Finalize() = 0;
    
    
  protected:
    std::string _name;
        
  private:
    
    const InterTTreeManager* _ttree_mgr_ptr;
    const InterImageManager* _img_mgr_ptr;
    const InterDataManager* _data_mgr_ptr;

    
  };

}


#endif
