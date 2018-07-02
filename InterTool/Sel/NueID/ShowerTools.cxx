#ifndef __SHOWERTOOLS_CXX__
#define __SHOWERTOOLS_CXX__

#include "ShowerTools.h"
#include "LArUtil/GeometryHelper.h"
#include "InterTool_Util/InterImageUtils.h"
#include <cassert>

namespace llcv {

  // adapted from https://goo.gl/Q7MBpE

  void ShowerTools::ReconstructAngle(const std::vector<larcv::Image2D*>& img_v,
				     const std::array<cv::Mat,3>& aimg_v,
				     Object2DCollection& obj_col) {

    auto geomH = larutil::GeometryHelper::GetME();
    
    // planes with largest number of hits used to get 3D direction
    std::vector<int> planeHits(3,0);
    std::vector<larutil::Point2D> planeDir(3);
    
    for(const auto& obj : obj_col) {

      const auto pl = obj._plane;
      
      larutil::Point2D weightedDir;
      weightedDir.w = 0;
      weightedDir.t = 0;
      float Qtot = 0;
      
      int nhits = 0;
      for(const auto& poly : obj._polygon_v) {
	auto nz_pt_v = larocv::FindNonZero(larocv::MaskImage(aimg_v[pl],poly.Contour(),-1,false));
	
	for (const auto& nz_pt  : nz_pt_v){
	  float charge = MatToImage2DPixel(nz_pt,aimg_v[pl],*(img_v[pl]));
	  if (charge<=0) continue;
	  weightedDir.w += (nz_pt.y - obj.Start().y) * charge;
	  weightedDir.t += (nz_pt.x - obj.Start().x) * charge;
	  Qtot += charge;
	  nhits++;
	}
      }
      
      weightedDir.w /= Qtot;
      weightedDir.t /= Qtot;

      planeHits[pl] = nhits;
      planeDir[pl]  = weightedDir;
    }

    int pl_max = larlite::data::kINVALID_INT;
    int pl_mid = larlite::data::kINVALID_INT;
    int pl_min = larlite::data::kINVALID_INT;

    int n_max  = -1.0*larlite::data::kINVALID_INT;
    int n_min  =      larlite::data::kINVALID_INT;

    for (size_t pl=0; pl < planeHits.size(); pl++){
      if (planeHits[pl] > n_max){
	pl_max = pl;
	n_max  = planeHits[pl];
      }
      if (planeHits[pl] < n_min){
	pl_min = pl;
	n_min  = planeHits[pl];
      }
    }

    assert(pl_max != larlite::data::kINVALID_INT);
    assert(pl_min != larlite::data::kINVALID_INT);

    // find the medium plane
    for(int pp=0; pp<3; ++pp) {
      if (pp == pl_max) continue;
      if (pp == pl_min) continue;
      pl_mid = pp;
    }

    assert(pl_mid != larlite::data::kINVALID_INT);

    float slope_max, slope_mid;
    float angle_max, angle_mid;
    slope_max = planeDir[pl_max].t / planeDir[pl_max].w;
    angle_max = std::atan(slope_max);
    angle_max = std::atan2( planeDir[pl_max].t , planeDir[pl_max].w );
    slope_mid = planeDir[pl_mid].t / planeDir[pl_mid].w;
    angle_mid = std::atan(slope_mid);
    angle_mid = std::atan2( planeDir[pl_mid].t , planeDir[pl_mid].w );
    
    double theta, phi;
    geomH->Get3DAxisN(pl_max, pl_mid,
		      angle_max, angle_mid,
		      phi, theta);
    
    obj_col.SetTheta(theta);
    obj_col.SetPhi(phi);
    
  }

  // adapted from https://goo.gl/1pCDEa

  void ShowerTools::ReconstructLength(const std::vector<larcv::Image2D*>& img_v,
				      const std::array<cv::Mat,3>& aimg_v,
				      Object2DCollection& obj_col) {
    
    auto geomH = larutil::GeometryHelper::GetME();

    // output
    std::array<double,3> length_v;

    // geometry
    std::array<double,3> plane_f_v;
    std::array<TVector2,3> line_dir_v;

    //initialize
    for(auto& v : length_v) 
      v = -1.0*::larlite::data::kINVALID_DOUBLE;
    
    plane_f_v = length_v;
    
    // calcluate line
    float alpha = 5;

    TVector3 dir3D(obj_col.dX(),obj_col.dY(),obj_col.dZ());
    
    for(size_t plane=0; plane<3; ++plane) {
      plane_f_v[plane] = geomH->Project_3DLine_OnPlane(dir3D, plane).Mag();
      
      const auto& startPoint2D = obj_col.front().Start();
      TVector3 secondPoint3D = obj_col.Start() + alpha * dir3D;
      
      int px_x, px_y;
      ProjectMat(img_v[plane]->meta(),
		 secondPoint3D.X(),secondPoint3D.Y(),secondPoint3D.Z(),
		 px_x, px_y);
      
      geo2d::Vector<float> secondPoint2D(px_y,px_x);
      LLCV_DEBUG() << "@plane=" << plane << " start=" << startPoint2D << " end=" << secondPoint2D << std::endl;

      TVector2 dir(secondPoint2D.y - startPoint2D.y, secondPoint2D.x - startPoint2D.x);
      dir *= 1.0 / dir.Mod();
      line_dir_v[plane] = dir;
    }

    // calculate the length
    for(const auto& obj : obj_col) {

      const auto pl = obj._plane;

      const auto& dr_w = line_dir_v[pl].X();
      const auto& dr_t = line_dir_v[pl].Y();
      
      std::vector<std::pair<float,float> > dist_v;

      float qsum = 0;
      float d2D = 0;
      
      for(const auto& poly : obj._polygon_v) {
	auto nz_pt_v = larocv::FindNonZero(larocv::MaskImage(aimg_v[pl],poly.Contour(),-1,false));
	for (const auto& nz_pt  : nz_pt_v) {
	  float charge = MatToImage2DPixel(nz_pt,aimg_v[pl],*(img_v[pl]));
	  if (charge<=0) continue;
	  qsum += charge;
	}
      }

      for(const auto& poly : obj._polygon_v) {
	auto nz_pt_v = larocv::FindNonZero(larocv::MaskImage(aimg_v[pl],poly.Contour(),-1,false));
     
	for(size_t nid=0; nid < nz_pt_v.size(); ++nid) {
	  auto nz_pt = nz_pt_v[nid];

	  float charge = MatToImage2DPixel(nz_pt,aimg_v[pl],*(img_v[pl]));

	  if (charge<=0) continue;
	  
	  float ptw = (nz_pt.y - obj.Start().y)*0.3;
	  float ptt = (nz_pt.x - obj.Start().x)*0.3;
	
	  // calculate distance along the line
	  d2D  = (ptw * dr_w) + (ptt * dr_t);
	  d2D  = std::abs(d2D);
	  dist_v.emplace_back(std::make_pair(d2D, charge / qsum));
	}
      } // end "hit" loop
      
      std::sort(std::begin(dist_v),std::end(dist_v),
		[](const std::pair<float,float>& lhs, const std::pair<float,float>& rhs)
		{ return lhs.first < rhs.first; });
      
      qsum = 0;
      d2D = 0;
      float _qfraction = 1.0;
      for(const auto& dist_pair : dist_v) {
	d2D   = dist_pair.first;
	qsum += dist_pair.second;
	if (qsum > _qfraction) break;
      }
      
      auto f = plane_f_v.at(pl);
      
      double length = d2D / f;

      LLCV_DEBUG() << "@pl=" << pl << " f=" << f << " length=" << length << std::endl;
      length_v[pl] = length;
    }
    
    float sum = 0.0;
    float nplanes = 0.0;

    for(size_t plane=0; plane<3; ++plane) {
      const auto length = length_v[plane];
      
      if (length == -1.0*::larlite::data::kINVALID_DOUBLE)
	continue;

      auto& obj2d = obj_col.PlaneObjectRW(plane);
      obj2d._length = length;

      nplanes += 1;
      sum += (float)length;
    }
    
    sum /= nplanes;

    obj_col.SetLength(sum);
  }


  // adapted from https://goo.gl/fb894P


  void ShowerTools::ReconstructdQdx(const std::vector<larcv::Image2D*>& img_v,
				    const std::array<cv::Mat,3>& aimg_v,
				    Object2DCollection& obj_col,
				    double dtrunk) {
    
    auto geomHelper = larutil::GeometryHelper::GetME();
    
    TVector3 dir3D(obj_col.dX(),obj_col.dY(),obj_col.dZ());
    
    std::array<double,3> plane_f_v;
    std::array<double,3> pitch_v;
    
    for(size_t plane=0; plane<3; ++plane) {
      plane_f_v[plane] = geomHelper->Project_3DLine_OnPlane(dir3D, plane).Mag();
      pitch_v[plane]   = geomHelper->GetPitch(dir3D,plane);
    }
    
    // loop through planes
    for(auto& obj : obj_col) {

      const auto pl = obj._plane;

      // grab the 2D start point of the cluster
      const auto& start2D = obj.Start();
      
      double f      = plane_f_v[pl];
      double pitch  = pitch_v[pl];

      std::vector<double> dqdx_v(3*dtrunk, 0.);
      
      // loop through hits and find those within some radial distance of the start point
      // loop over hits
      size_t nhits = 0;
      for(const auto& poly : obj._polygon_v) {
	auto nz_pt_v = larocv::FindNonZero(larocv::MaskImage(aimg_v[pl],poly.Contour(),-1,false));

	for(size_t nid=0; nid < nz_pt_v.size(); ++nid) {
	  const auto& nz_pt = nz_pt_v[nid];

	  double d2D = std::sqrt( std::pow((nz_pt.y - start2D.y)*0.3, 2) + 
				  std::pow((nz_pt.x - start2D.x)*0.3, 2) );
	  
	  double d3D = d2D / f;
	  
	  if (d3D >= dtrunk) continue;
	  
	  auto charge = MatToImage2DPixel(nz_pt,aimg_v[pl],*(img_v[pl]));

	  if (charge <= 0) continue;

	  dqdx_v.at((size_t)(d3D * 3)) += charge;

	  nhits++;
	} // end "hit"
      } // loop over all polygons

      static std::vector<double> dqdx_nonzero_v;
      dqdx_nonzero_v.clear();
      dqdx_nonzero_v.reserve(nhits);

      for (const auto& dqdx : dqdx_v) {
	if (dqdx) dqdx_nonzero_v.push_back(dqdx); 
      }
      
      double dqdx = -1.0*larlite::data::kINVALID_DOUBLE;
      
      if (dqdx_nonzero_v.empty()) dqdx = 0.;
      else {

	size_t half_idx = (size_t)(((float)dqdx_nonzero_v.size()) / 2.0);

	std::nth_element(dqdx_nonzero_v.begin(),
			 dqdx_nonzero_v.begin() + half_idx,
			 dqdx_nonzero_v.end());

	dqdx  = dqdx_nonzero_v.at(half_idx);
	dqdx /= pitch;
      }
      
      obj._dqdx = dqdx;
      
    } // end plane
    
    return;
  }
  
  

}


#endif
