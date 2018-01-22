#ifndef __INTERMANAGER_H__
#define __INTERMANAGER_H__

//llcv
#include "LLCVBase/llcv_err.h"
#include "LLCVBase/llcv_base.h"

//me
#include "InterToolTypes.h"
#include "InterImage.h"

//locv
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"

//lcv
#include "DataFormat/Image2D.h"
#include "LArbysImageMaker.h"

//ll
#include "DataFormat/track.h"
#include "DataFormat/shower.h"

//cpp
#include <map>

namespace llcv {

  class InterManager : public llcv_base {

  public:
  InterManager() : llcv_base() {}
    ~InterManager(){}
    
    void SetImage(const std::vector<larcv::Image2D>& img_v, llcv::InterImageType iitype);
    void Reset();

  private:

    std::vector<larlite::track*> Tracks();
    std::vector<larlite::shower*> Showers();
    
    std::vector<cv::Mat*> 
      OCVImage(llcv::InterImageType iitype=kADC, int cropx=-1, int cropy=-1);

    void InitializeOCVImages();
    void CropImages(int cropx, int cropy);

    std::map<std::pair<int,int>, std::vector<InterImage> > _inter_adc_m;
    std::map<std::pair<int,int>, std::vector<InterImage> > _inter_shr_m;
    std::map<std::pair<int,int>, std::vector<InterImage> > _inter_trk_m;

    larocv::data::Vertex3D _vtx;
    std::vector<std::pair<int,int> > _vtx_pixel_v;

    larcv::LArbysImageMaker _larmkr;
  };

}

#endif
