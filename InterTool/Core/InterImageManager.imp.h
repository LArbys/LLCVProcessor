#ifndef __INTERIMAGEMANAGER_IMP_H__
#define __INTERIMAGEMANAGER_IMP_H__

#include "InterImageManager.h"

namespace llcv {

  template <class T>
  std::vector<T*> InterImageManager::Image(llcv::InterImageType iitype, int cropx, int cropy) {
    LLCV_DEBUG() << "start" << std::endl;     
    std::pair<int,int> cpair = std::make_pair(cropx,cropy);

    SetIIT(iitype,cpair);

    if ((*_iimg_v).empty())
      CropImage(cpair,iitype);
    
    std::vector<T*> res_v;
    res_v.reserve((*_iimg_v).size());
    for(auto& iimg : *_iimg_v) 
      res_v.push_back(iimg.get<T>());

    LLCV_DEBUG() << "end" << std::endl;
    return res_v;
  }

  template <class T>
    std::vector<T*> InterImageManager::RawImage(llcv::InterImageType iitype) {
    LLCV_DEBUG() << "return raw" << std::endl;
    std::pair<int,int> cpair = std::make_pair(-1,-1);
    std::vector<InterImage>* iimg_v = NULL;
    switch( iitype ) {
    case kImageADC:
      iimg_v = &( _inter_adc_m.find(cpair)->second );
      break;
    case kImageTrack:
      iimg_v = &( _inter_trk_m.find(cpair)->second );      
      break;
    case kImageShower:
      iimg_v = &( _inter_shr_m.find(cpair)->second );
      break;
    default:
      throw std::runtime_error("asking for unrecognized interimage type");
      break;
    }

    std::vector<T*> res_v;
    res_v.reserve((*iimg_v).size());
    for(auto & iimg : *iimg_v) 
      res_v.push_back(iimg.get<T>());
    
    return res_v;
  }
  

}

#endif
