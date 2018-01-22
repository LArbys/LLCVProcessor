#ifndef __INTERMANAGER_CXX__
#define __INTERMANAGER_CXX__

#include "InterManager.h"

#include <cassert>

namespace llcv {

  void InterManager::InitializeOCVImages() {

    auto& adc_iimg_v = _inter_adc_m.begin()->second;
    auto& trk_iimg_v = _inter_trk_m.begin()->second;
    auto& shr_iimg_v = _inter_shr_m.begin()->second;
    
    for(size_t plane=0; plane<3; ++plane) {
      auto& adc_iimg  = adc_iimg_v.at(plane);
      auto& trk_iimg  = trk_iimg_v.at(plane);
      auto& shr_iimg  = shr_iimg_v.at(plane);

      auto adc_mat_meta = _larmkr.ExtractImage(adc_iimg.img2d);
      auto trk_mat_meta = _larmkr.ExtractImage(trk_iimg.img2d);
      auto shr_mat_meta = _larmkr.ExtractImage(shr_iimg.img2d);

      adc_iimg.mat = std::move(std::get<0>(adc_mat_meta));
      trk_iimg.mat = std::move(std::get<0>(trk_mat_meta));
      shr_iimg.mat = std::move(std::get<0>(shr_mat_meta));

      adc_iimg.meta = std::get<1>(adc_mat_meta);
      trk_iimg.meta = std::get<1>(trk_mat_meta);
      shr_iimg.meta = std::get<1>(shr_mat_meta);
    }


    return;
  }
  
  void InterManager::CropImages(int cropx, int cropy) {
    
    auto& adc_iimg_v = _inter_adc_m.begin()->second;
    auto& shr_iimg_v = _inter_shr_m.begin()->second;
    auto& trk_iimg_v = _inter_trk_m.begin()->second;
    
    std::pair<int,int> cpair = std::make_pair(cropx,cropy);
    auto& adc_crop_iimg_v = _inter_adc_m[cpair];
    auto& shr_crop_iimg_v = _inter_shr_m[cpair];
    auto& trk_crop_iimg_v = _inter_trk_m[cpair];
    
    adc_crop_iimg_v.resize(3);
    shr_crop_iimg_v.resize(3);
    trk_crop_iimg_v.resize(3);

    // crop the image by defined number of pixels left and right in user config
    for(size_t plane=0; plane<3; ++plane) {

      auto row = _vtx_pixel_v[plane].first;
      auto col = _vtx_pixel_v[plane].second;

      const auto& adc_img  = adc_iimg_v.at(plane).img2d;
      const auto& shr_img  = shr_iimg_v.at(plane).img2d;
      const auto& trk_img  = trk_iimg_v.at(plane).img2d;

      const auto& meta = adc_img.meta();

      LLCV_DEBUG() << "@plane=" << plane << std::endl;
      LLCV_DEBUG() << "(row,col)="<< row << "," << col << ")" << std::endl;
      LLCV_DEBUG() << "pixel (width,height)=(" << meta.pixel_width() << "," << meta.pixel_height() << ")" << std::endl;

      double width  = cropx*meta.pixel_width();
      double height = cropy*meta.pixel_height();

      LLCV_DEBUG() << "(width,height)=("<< width << "," << height << ")" << std::endl;

      size_t row_count = (size_t) cropx;
      size_t col_count = (size_t) cropy;

      LLCV_DEBUG() << "(rows,cols)=("<< row_count << "," << col_count << ")" << std::endl;
      LLCV_DEBUG() << "origin (x,y)=( " << meta.tl().x << "," << meta.tl().y << ")" << std::endl;

      double origin_x = meta.tl().x + meta.pixel_width()  * ((double)col);
      double origin_y = meta.tl().y - meta.pixel_height() * ((double)row);

      LLCV_DEBUG() << "0 (" << origin_x << "," << origin_y << ")" << std::endl;

      origin_x -= width/2.0;
      origin_y += height/2.0;

      LLCV_DEBUG() << "tl: " << meta.tl().x << "," << meta.tl().y << std::endl;
      LLCV_DEBUG() << "tr: " << meta.tr().x << "," << meta.tr().y << std::endl;
      LLCV_DEBUG() << "bl: " << meta.bl().x << "," << meta.bl().y << std::endl;
      LLCV_DEBUG() << "br: " << meta.br().x << "," << meta.br().y << std::endl;
	
      // check if origin is on the edge, move it to the edge if needed
      if (origin_x < meta.tl().x) origin_x = meta.tl().x;
      if (origin_x > meta.tr().x) origin_x = meta.tr().x;

      if (origin_y > meta.tl().y) origin_y = meta.tl().y;
      if (origin_y < meta.br().y) origin_y = meta.br().y;

      // check if origin on the other edge, move if on the edge
      LLCV_DEBUG() << "2 (" << origin_x << "," << origin_y << ")" << std::endl;

      auto max_x = origin_x + width;
      auto min_y = origin_y - height;

      if (max_x > meta.max_x()) {
	auto dist = meta.max_x() - max_x;
	origin_x += dist;
      }

      if (min_y < meta.min_y()) {
	auto dist = meta.min_y() - min_y;
	origin_y += dist;
      }

      LLCV_DEBUG() << "3 (" << origin_x << "," << origin_y << ")" << std::endl;

      larcv::ImageMeta crop_meta(width,height,
				 row_count,col_count,
				 origin_x,origin_y,
				 plane);
	
      LLCV_DEBUG() << meta.dump();
      LLCV_DEBUG() << crop_meta.dump();

      auto& adc_crop_iimg = adc_crop_iimg_v[plane];
      auto& trk_crop_iimg = trk_crop_iimg_v[plane];
      auto& shr_crop_iimg = shr_crop_iimg_v[plane];
      
      adc_crop_iimg.img2d = adc_img.crop(crop_meta);
      trk_crop_iimg.img2d = trk_img.crop(crop_meta);
      shr_crop_iimg.img2d = shr_img.crop(crop_meta);

      auto adc_mat_meta = _larmkr.ExtractImage(adc_crop_iimg.img2d);
      auto trk_mat_meta = _larmkr.ExtractImage(trk_crop_iimg.img2d);
      auto shr_mat_meta = _larmkr.ExtractImage(shr_crop_iimg.img2d);

      adc_crop_iimg.mat = std::move(std::get<0>(adc_mat_meta));
      trk_crop_iimg.mat = std::move(std::get<0>(trk_mat_meta));
      shr_crop_iimg.mat = std::move(std::get<0>(shr_mat_meta));

      adc_crop_iimg.meta = std::get<1>(adc_mat_meta);
      trk_crop_iimg.meta = std::get<1>(trk_mat_meta);
      shr_crop_iimg.meta = std::get<1>(shr_mat_meta);
    }

    
    return;
  }


  std::vector<cv::Mat*> InterManager::OCVImage(llcv::InterImageType iitype, int cropx, int cropy) {
    std::pair<int,int> crop = std::make_pair(cropx,cropy);
    auto iter = _inter_adc_m.find(crop);
    
    if (iter == _inter_adc_m.end()) 
      CropImages(cropx,cropy);
    
    iter = _inter_adc_m.find(crop);
    assert (iter != _inter_adc_m.end());
    
    std::vector<InterImage>* iimg_v;
    switch (iitype) {
    case kADC: iimg_v = &iter->second;  break;
    case kSHR: iimg_v = &_inter_shr_m.at(crop); break;
    case kTRK: iimg_v = &_inter_trk_m.at(crop); break;
    default: throw llcv_err("invalid InterImageTypeRequested");
    }
    
    std::vector<cv::Mat*> res_v;
    res_v.reserve(iimg_v->size());
    for(auto& iimg : *iimg_v) res_v.push_back(&(iimg.mat));
    
    return res_v;
  }


  void InterManager::SetImage(const std::vector<larcv::Image2D>& img_v, llcv::InterImageType iitype) {

    std::pair<int,int> cpair = std::make_pair(-1,-1);
    auto& adc_iimg_v = _inter_adc_m[cpair];
    auto& trk_iimg_v = _inter_trk_m[cpair];
    auto& shr_iimg_v = _inter_shr_m[cpair];
    
    if (adc_iimg_v.size()<3) adc_iimg_v.resize(3);
    if (trk_iimg_v.size()<3) trk_iimg_v.resize(3);
    if (shr_iimg_v.size()<3) shr_iimg_v.resize(3);

    for(size_t plane=0; plane<3; ++plane) {
      
      switch (iitype) {
      case kADC: 
	adc_iimg_v[plane].img2d = img_v[plane];
	break;
	
      case kTRK:
	trk_iimg_v[plane].img2d = img_v[plane];
	break;

      case kSHR:
	shr_iimg_v[plane].img2d = img_v[plane];
	break;
	
      default: 
	throw llcv_err("invalid InterImageTypeRequested");
      }
    }

    InitializeOCVImages();
    
  }

  void InterManager::Reset() {

    _inter_adc_m.clear();
    _inter_shr_m.clear();
    _inter_trk_m.clear();

    _vtx_pixel_v.clear();
    _vtx_pixel_v.resize(3);

  }
  
}

#endif
