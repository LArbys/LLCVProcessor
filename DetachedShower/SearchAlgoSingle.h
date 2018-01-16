#ifndef __SEARCHALGOSINGLE_H__
#define __SEARCHALGOSINGLE_H__

#include "SearchAlgoBase.h"

namespace llcv {

  class SearchAlgoSingle : public SearchAlgoBase {
  public:
    SearchAlgoSingle() : SearchAlgoBase("SearchAlgoSingle") {}
    ~SearchAlgoSingle() {}
    
    std::vector<llcv::DetachedCandidate>
    Search(const larocv::data::Vertex3D& vtx3d,
	   const std::vector<std::tuple<cv::Mat,larocv::ImageMeta> >& mat_meta_v);
  };

}
#endif
