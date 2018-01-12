#ifndef SEARCHALGOBASE_H
#define SEARCHALGOBASE_H

#include "LArOpenCV/Core/ImageMeta.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"

namespace llcv {
class SearchAlgoBase {
  
 public:
  SearchAlgoBase();
  virtual ~SearchAlgoBase();

  virtual bool 
    Search(const larocv::data::Vertex3D& vtx3d,
	   const std::vector<std::tuple<cv::Mat,larocv::ImageMeta> >& mat_meta_v) = 0;

};
}
#endif
