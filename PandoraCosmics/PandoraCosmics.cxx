#ifndef PANDORACOSMICS_CXX
#define PANDORACOSMICS_CXX

// llcv
#include "PandoraCosmics.h"

// larcv
#include "DataFormat/EventImage2D.h"

// larlite
#include "DataFormat/hit.h"

#include "CVUtil/CVUtil.h"
#include <array>
#include <cassert>

namespace llcv {

  void PandoraCosmics::configure(const larcv::PSet& cfg) {
    LLCV_DEBUG() << "start" << std::endl;

    LLCV_DEBUG() << "end" << std::endl;
  }

  void PandoraCosmics::initialize() {
    LLCV_DEBUG() << "start" << std::endl;

    _tree = new TTree("PandoraCosmics","");
    _tree->Branch("run"     , &_run     , "run/I");
    _tree->Branch("subrun"  , &_subrun  , "subrun/I");
    LLCV_DEBUG() << "end" << std::endl;
  }

  bool PandoraCosmics::process(larcv::IOManager& mgr, larlite::storage_manager& sto) {
    LLCV_DEBUG() << "start" << std::endl;

		auto ev_image = (larcv::EventImage2D*)mgr.get_data(larcv::kProductImage2D,"wire");		
		auto ev_hit  = (larlite::event_hit*)sto.get_data(larlite::data::kHit,"pandoraCosmicHitRemoval"); 

		// a quick dumb test:
		auto img_test  = as_mat(ev_image->Image2DArray().at(0));
		cv::imwrite("dumb.png",img_test);
		
		// Let's see if this works... something a little stupid.
		std::vector < cv::Mat > img_v = {cv::imread("all_black.png",CV_LOAD_IMAGE_GRAYSCALE),cv::imread("all_black.png",CV_LOAD_IMAGE_GRAYSCALE),cv::imread("all_black.png",CV_LOAD_IMAGE_GRAYSCALE)};	

		//std::vector < cv::Mat > img_v = {as_gray_mat(ev_image->Image2DArray().at(0)),  as_gray_mat(ev_image->Image2DArray().at(1)), as_gray_mat(ev_image->Image2DArray().at(2))};

		// Let's see if we can map pixels to hits
		size_t plane=0;
    double wire=0;
		double time=0;
		double px_x  = 0;
		double px_y = 0;

		int cnt = 0;
		for(auto const& h : *ev_hit) {
			size_t plane = h.WireID().Plane;	
			// Get time and wire coords from hit
			time = h.PeakTime() + 2400;
			wire = h.WireID().Wire;
			auto meta = ev_image->Image2DArray().at(plane).meta();
			px_x = (wire - meta.min_x()) / meta.pixel_width();
			px_y = (meta.max_y() - time) / meta.pixel_height();

			// um... for now, let's draw and see what that looks like, yeah?
			// TODO: play with line width to see how things get affected
			cv::circle(img_v[plane],cv::Point((int)px_x,(int)px_y),int(0),cv::Scalar(255),1);
		}		
	

		for(int i = 0; i < 3; i++){
			cv::imwrite("afta_"+std::to_string(i)+".png",img_v[i]);
			//std::cout << img_v[i].channels() << std::endl;;
		}

		auto ev_img2d_new = (larcv::EventImage2D*)mgr.get_data(larcv::kProductImage2D,"new_producer");
		ev_img2d_new->Append(larcv::mat_to_image2d(img_v[0]));
		ev_img2d_new->Append(larcv::mat_to_image2d(img_v[1]));
		ev_img2d_new->Append(larcv::mat_to_image2d(img_v[2]));
				
    LLCV_DEBUG() << "end" << std::endl;
    return true;
  }
  
  void PandoraCosmics::finalize() {
    LLCV_DEBUG() << "start" << std::endl;

    _tree->Write();

    LLCV_DEBUG() << "end" << std::endl;
  }

}

#endif
	
