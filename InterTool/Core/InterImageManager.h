#ifndef __INTERIMAGEMANAGER_H__
#define __INTERIMAGEMANAGER_H__

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

//cpp
#include <map>

namespace llcv {

  class InterImageManager : public llcv_base {

  public:
  InterImageManager(std::string name="InterImageManager") : 
    llcv_base(name), 
      _name(name) ,
      _iimg_v(nullptr)
      {}

    ~InterImageManager(){}
    
    void SetImage(const std::vector<larcv::Image2D>& img_v, llcv::InterImageType iitype);
    void SetVertex(float x, float y, float z) {}
    void Reset();

    std::vector<std::pair<int,int> > _vtx_pixel_v;

    std::vector<cv::Mat*> 
      OCVImage(llcv::InterImageType iitype=kADC, int cropx=-1, int cropy=-1);

  private:

    std::string _name;

    void SetIIT(InterImageType iitype,const std::pair<int,int>& cpair);
    void SetIIT(InterImageType iitype,int cropx, int cropy);

    void InitializeOCVImage(InterImageType iitype);
    void CropImage(int cropx, int cropy,InterImageType iitype);

    std::map<std::pair<int,int>, std::vector<InterImage> > _inter_adc_m;
    std::map<std::pair<int,int>, std::vector<InterImage> > _inter_shr_m;
    std::map<std::pair<int,int>, std::vector<InterImage> > _inter_trk_m;

    larocv::data::Vertex3D _vtx;

    larcv::LArbysImageMaker _larmkr;
    
    std::vector<InterImage>* _iimg_v;
  };

}

#endif
