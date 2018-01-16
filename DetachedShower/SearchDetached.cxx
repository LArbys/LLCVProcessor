#ifndef SEARCHDETACHED_CXX
#define SEARCHDETACHED_CXX

// llcv
#include "SearchDetached.h"

// larcv
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventPGraph.h"
#include "DataFormat/EventPixel2D.h"

// laropencvhandl
#include "RecoTruthMatch/MatchUtils.h"

// locv

// larlite
#include "DataFormat/vertex.h"
#include "DataFormat/pfpart.h"
#include "DataFormat/shower.h"

#include <cassert>

namespace llcv {

  void SearchDetached::configure(const larcv::PSet& cfg) {
    LLCV_DEBUG() << "start" << std::endl;

    _adc_img_prod = cfg.get<std::string>("ADCImageProducer");
    _shr_img_prod = cfg.get<std::string>("ShowerImageProducer");

    _pgraph_prod  = cfg.get<std::string>("PGraphProducer");
    _pixel_prod   = cfg.get<std::string>("Pixel2DProducer");

    _out_pgraph_prod = cfg.get<std::string>("OutPGraphProducer");
    _out_pixel_prod  = cfg.get<std::string>("OutPixel2DProducer");

    _search_distance = cfg.get<float>("SearchDistance");

    _larmkr.Configure(cfg.get<larcv::PSet>("LArbysImageMaker"));

    LLCV_DEBUG() << "end" << std::endl;
  }

  void SearchDetached::initialize() {
    LLCV_DEBUG() << "start" << std::endl;


    LLCV_DEBUG() << "end" << std::endl;
  }

  bool SearchDetached::process(larcv::IOManager& mgr, larlite::storage_manager& sto) {
    LLCV_DEBUG() << "start" << std::endl;

    auto ev_adc_img = (larcv::EventImage2D*)mgr.get_data(larcv::kProductImage2D,_adc_img_prod);
    auto ev_shr_img = (larcv::EventImage2D*)mgr.get_data(larcv::kProductImage2D,_shr_img_prod);
    
    auto ev_pgraph  = (larcv::EventPGraph*) mgr.get_data(larcv::kProductPGraph ,_pgraph_prod);
    auto ev_pixel   = (larcv::EventPixel2D*)mgr.get_data(larcv::kProductPixel2D,_pixel_prod);

    auto ev_out_pgraph = (larcv::EventPGraph*) mgr.get_data(larcv::kProductPGraph ,_out_pgraph_prod);
    auto ev_out_pixel  = (larcv::EventPixel2D*)mgr.get_data(larcv::kProductPixel2D,_out_pixel_prod);

    //
    // mask the particle image
    //
    auto shr_img_v = ev_adc_img->Image2DArray();
    const auto& pgraph_v   = ev_pgraph->PGraphArray();
    const auto& pix_m      = ev_pixel->Pixel2DClusterArray();
    const auto& pix_meta_m = ev_pixel->ClusterMetaArray();

    MaskImage(pgraph_v,pix_m,pix_meta_m,shr_img_v);

    // 
    // search for the other shower per vertex
    //

    std::vector<larcv::Image2D> shr_crop_img_v(3);
    for(size_t vtxid=1; vtxid < pgraph_v.size(); ++vtxid) {
      
      const auto& pgraph = pgraph_v[vtxid];
      const auto& par = pgraph.ParticleArray().front();
      auto vtx_X = par.X();
      auto vtx_Y = par.Y();
      auto vtx_Z = par.Z();

      LLCV_DEBUG() << "@vtxid=" << vtxid 
		   << " (" << vtx_X << "," << vtx_Y << "," << vtx_Z << ")" << std::endl;

      // project 3d vertex into plane
      // crop image to specified dimension
      double xpixel = kINVALID_DOUBLE;
      double ypixel = kINVALID_DOUBLE;

      std::vector<std::pair<int,int> > vtx_pixel_v(3);

      for(size_t plane=0; plane<3; ++plane) {
	xpixel = ypixel = kINVALID_DOUBLE;
	const auto& meta = shr_img_v[plane].meta();
	Project3D(meta,vtx_X,vtx_Y,vtx_Z,0.0,plane,xpixel,ypixel);
	int xx = (int)(xpixel+0.5);
	int yy = (int)(ypixel+0.5);
	yy = meta.rows() - yy - 1;
	vtx_pixel_v[plane] = std::make_pair(yy,xx);
	LLCV_DEBUG() << "@plane=" << plane << " (" << yy << "," << xx << ")" << std::endl;
      }

      // crop the image by defined number of pixels left and right
      for(size_t plane=0; plane<3; ++plane) {
	auto row = vtx_pixel_v[plane].first;
	auto col = vtx_pixel_v[plane].second;

	const auto& shr_img  = shr_img_v[plane];
	const auto& meta = shr_img.meta();

	std::cout << "@plane=" << plane << std::endl;
	std::cout << "(row,col)="<< row << "," << col << ")" << std::endl;

	double width  = _search_distance*meta.pixel_width();
	double height = _search_distance*meta.pixel_height();

	size_t row_count = (size_t) _search_distance;
	size_t col_count = (size_t) _search_distance;

	double origin_x = meta.pos_x(col);
	double origin_y = meta.pos_y(row);

	std::cout << "(" << origin_x << "," << origin_y << ")" << std::endl;

	origin_x -= width/2.0;
	origin_y += height/2.0;
	
	larcv::ImageMeta crop_meta(width,height,
				   row_count,col_count,
				   origin_x,origin_y,
				   plane);
	
	std::cout << "a" << std::endl;
	std::cout << meta.dump() << std::endl;
	std::cout << crop_meta.dump() << std::endl;

	shr_crop_img_v[plane] = shr_img.crop(crop_meta);
      }
      
      // auto mat0_v = _larmkr.ExtractMat(shr_img_v);
      // auto mat1_v = _larmkr.ExtractMat(shr_crop_img_v);
	
      // cv::imwrite("orig_2.png",mat0_v.back());
      // cv::imwrite("crop_2.png",mat1_v.back());

      auto mat_v = _larmkr.ExtractImage(shr_crop_img_v);
      
      larocv::data::Vertex3D vtx3d;
      vtx3d.x = vtx_X;
      vtx3d.y = vtx_Y;
      vtx3d.z = vtx_Z;
      auto ret_v = _algo->Search(vtx3d,mat_v);

      std::exit(1);
    }
    
    LLCV_DEBUG() << "end" << std::endl;
    return true;
  }
  
  void SearchDetached::MaskImage(const std::vector<larcv::PGraph>& pgraph_v,
				 const std::map<larcv::PlaneID_t, std::vector<larcv::Pixel2DCluster> >& pix_m,
				 const std::map<larcv::PlaneID_t, std::vector<larcv::ImageMeta> >&   pix_meta_m,
				 std::vector<larcv::Image2D>& img_v) {
    
    for(const auto& pgraph : pgraph_v) {
      auto const& roi_v         = pgraph.ParticleArray();
      auto const& cluster_idx_v = pgraph.ClusterIndexArray();

      for(size_t roid=0; roid < roi_v.size(); ++roid) {
	
	const auto& roi = roi_v[roid];
	auto cidx = cluster_idx_v.at(roid);

	for(size_t plane=0; plane<3; ++plane) {

	  auto iter_pix = pix_m.find(plane);
	  if(iter_pix == pix_m.end())
	    continue;

	  auto iter_pix_meta = pix_meta_m.find(plane);
	  if(iter_pix_meta == pix_meta_m.end())
	    continue;
	  
	  auto const& pix_v      = (*iter_pix).second;
	  auto const& pix_meta_v = (*iter_pix_meta).second;
	  
	  auto const& pix      = pix_v.at(cidx);
	  auto const& pix_meta = pix_meta_v.at(cidx);
	  
	  auto& plane_img = img_v.at(plane);

	  for(const auto& px : pix) {
	    auto posx = pix_meta.pos_x(px.Y());
	    auto posy = pix_meta.pos_y(px.X());
	    auto row = plane_img.meta().row(posy);
	    auto col = plane_img.meta().col(posx);
	    plane_img.set_pixel(row,col,0);
	  }
	} // end plane
      } // end roi
    } // end pgraph
    
  }
    

  
  void SearchDetached::finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    
    LLCV_DEBUG() << "end" << std::endl;
  }

}

#endif
	
