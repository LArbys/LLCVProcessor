#ifndef SEARCHALGOBASE_H
#define SEARCHALGOBASE_H

// llcv
#include "LLCVBase/llcv_base.h"
#include "DetachedCandidate.h"

// locv
#include "LArOpenCV/Core/ImageMeta.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"

namespace llcv {
  class SearchAlgoBase : public llcv_base {

  public:
  SearchAlgoBase(std::string name="SearchAlgoBase") : _name(name){}
    virtual ~SearchAlgoBase(){}

    const std::string& Name() { return _name; }

    virtual std::vector<llcv::DetachedCandidate>
      Search(const larocv::data::Vertex3D& vtx3d,
	     const std::vector<std::tuple<cv::Mat,larocv::ImageMeta> >& mat_meta_v) = 0;
    
  protected:
    std::string _name;

  };
}
#endif
