#ifndef DLHANDSHAKE_H
#define DLHANDSHAKE_H

#include "LLCVBase/AnaBase.h"
#include "HandShaker.h"

namespace llcv {

  class DLHandshake : public AnaBase {

  public:

  DLHandshake(const std::string name="DLHandshake") : AnaBase(name) {}
    ~DLHandshake() {}

    void configure(const larcv::PSet&);
    void initialize();
    bool process(larcv::IOManager& mgr, larlite::storage_manager& sto);
    void finalize();

  private:

    HandShaker _HandShaker;
    
  };

}

#endif
