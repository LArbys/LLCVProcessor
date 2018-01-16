#ifndef __SEARCHALGOSINGLE_CXX__
#define __SEARCHALGOSINGLE_CXX__

#include "SearchAlgoSingle.h"

namespace llcv {
  std::vector<llcv::DetachedCandidate>
    SearchAlgoSingle::Search(const larocv::data::Vertex3D& vtx3d,
			     const std::vector<std::tuple<cv::Mat,larocv::ImageMeta> >& mat_meta_v) {
  
    LLCV_DEBUG() << "start" << std::endl;
    std::vector<llcv::DetachedCandidate> res_v;
    res_v.resize(1);
    auto& res = res_v.front();
    
  

    LLCV_DEBUG() << "end" << std::endl;
    return res_v;
  }

}
#endif
