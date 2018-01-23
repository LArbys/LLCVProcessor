#ifndef __INTERMODULE_H__
#define __INTERMODULE_H__

//llcv
#include "LLCVBase/AnaBase.h"

#include "InterTool_Core/InterDriver.h"

namespace llcv {
  
class InterModule : public AnaBase {
    
public:
    
InterModule(const std::string name="InterModule") :  AnaBase(name) {}
~InterModule() {}
    
void configure(const larcv::PSet& cfg);
void initialize();
bool process(larcv::IOManager& mgr, larlite::storage_manager& sto);
void finalize();

};

}

#endif
