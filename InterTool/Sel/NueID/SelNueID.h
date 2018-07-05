#ifndef __SELNUEID_H__
#define __SELNUEID_H__

#include "InterTool_Core/InterSelBase.h"
#include "TStopwatch.h"
#include "ContourScan.h"
#include "InterTool_Util/Triangle.h"
#include "MatchObjectAlgoTimeIOU.h"
#include "Object2D.h"
#include "CosmicTag.h"
#include "LineExtension.h"
#include "ShowerTools.h"

namespace llcv {
  
  class SelNueID : public InterSelBase { 

  public:

  SelNueID(std::string name="SelNueID") : InterSelBase(name), _outtree(nullptr) 
    { _ismc = false; _debug = false; }

    ~SelNueID(){}
      
    void Configure (const larcv::PSet &pset);
    void Initialize();
    double Select();
    void Finalize();

    void SetIsMC(bool ismc) { _ismc = ismc; }
    void SetDebug(bool debug) { _debug = debug; }
    
  private:
    
    TTree* _outtree;
    
    size_t _cropx;
    size_t _cropy;
    size_t _n_neighbors;
    float _brem_dist;
    int _brem_size;

    float _extension_cutoff;
    
    ShowerTools _ShowerTools;

    ContourScan _ContourScan;

    TStopwatch _twatch;

    cv::Mat _white_img;

    MatchObjectAlgoTimeIOU _Match;

    std::vector<CosmicTag> _CosmicTag_v;
    std::vector<LineExtension> _LineExtension_v;

  private:

    void ResetEvent();
    void ResizePlanePolygon(size_t sz);
    void SetParticlePlane(size_t pid, size_t plane);
    void SetSegmentPlane(size_t pid, size_t plane);
    void SetParticle(size_t pid);

    size_t FindClosestContour(const larocv::GEO2D_ContourArray_t& ctor_v,
			      const geo2d::Vector<float>& pt,
			      float& distance);

    larocv::GEO2D_Contour_t MaximizeLine(const cv::Mat& timg3d_par,
					 const Triangle& triangle,
					 float& nline_pixels,
					 geo2d::Vector<float>& edge);
    
    
    larocv::GEO2D_Contour_t MaximizeTriangleLine(const cv::Mat& timg3d_mask,
						 const Triangle& triangle,
						 float& nline_pixels,
						 float& npar_pixels,
						 geo2d::Vector<float>& edge);
    
    larocv::GEO2D_Contour_t MaximizePolygonLine(const cv::Mat& timg3d_mask,
						const std::vector<Polygon>& polygon_v,
						Triangle& triangle,
						float& nline_pixels,
						float& npar_pixels,
						geo2d::Vector<float>& edge);

    float MinimizeToEdge(const larocv::GEO2D_Contour_t& ctor);
    
    float NearestPolygonToCosmic(const std::vector<Polygon>& polygon_v,size_t plane);

    int DetectBrem(Triangle& triangle, 
		   const larocv::GEO2D_ContourArray_t& other_ctor_v,
		   std::vector<size_t>& id_v,
		   std::vector<size_t>& inside_v);
    
    
  private:
    
    bool _ismc;
    bool _debug;

    float _vertex_x;
    float _vertex_y;
    float _vertex_z;
    
    int _n_par;
    std::vector<float> _par_score_v;

    int _vtx_xing_U;
    int _vtx_xing_V;
    int _vtx_xing_Y;

    std::vector<float> _vtx_linelen_U_v;
    std::vector<float> _vtx_linelen_V_v;
    std::vector<float> _vtx_linelen_Y_v;

    std::vector<float> _vtx_linefrac_U_v;
    std::vector<float> _vtx_linefrac_V_v;
    std::vector<float> _vtx_linefrac_Y_v;

    std::vector<float> _edge_dist_v;
    std::vector<int>   _edge_n_cosmic_v;
    std::vector<float> _edge_cosmic_vtx_dist_v;

    float _par1_theta;
    float _par1_phi;
    float _par1_length;
    float _par1_score;
    float _par1_dx;
    float _par1_dy;
    float _par1_dz;
    int   _par1_nplanes;
    std::vector<int>   _par1_planes_v;
    std::vector<int>   _par1_xdead_v;
    std::vector<float> _par1_cosmic_dist_v;

    float _par2_theta;
    float _par2_phi;
    float _par2_length;
    float _par2_score;
    float _par2_dx;
    float _par2_dy;
    float _par2_dz;
    int   _par2_nplanes;
    std::vector<int>   _par2_planes_v;
    std::vector<int>   _par2_xdead_v;
    std::vector<float> _par2_cosmic_dist_v;

    float* _par_theta;
    float* _par_phi;
    float* _par_length;
    float* _par_score;
    float* _par_dx;
    float* _par_dy;
    float* _par_dz;
    int*   _par_nplanes;
    std::vector<int>*   _par_planes_v;
    std::vector<int>*   _par_xdead_v;
    std::vector<float>* _par_cosmic_dist_v;

    int _par1_n_polygons_U;
    int _par1_n_polygons_V;
    int _par1_n_polygons_Y;
    int _par2_n_polygons_U;
    int _par2_n_polygons_V;
    int _par2_n_polygons_Y;
    int* _par_n_polygons;

    float _par1_linelength_U;
    float _par1_linelength_V;
    float _par1_linelength_Y;
    float _par2_linelength_U;
    float _par2_linelength_V;
    float _par2_linelength_Y;
    float* _par_linelength;

    float _par1_linefrac_U;
    float _par1_linefrac_V;
    float _par1_linefrac_Y;
    float _par2_linefrac_U;
    float _par2_linefrac_V;
    float _par2_linefrac_Y;
    float* _par_linefrac;

    float _par1_linedx_U;
    float _par1_linedx_V;
    float _par1_linedx_Y;
    float _par2_linedx_U;
    float _par2_linedx_V;
    float _par2_linedx_Y;
    float* _par_linedx;

    float _par1_linedy_U;
    float _par1_linedy_V;
    float _par1_linedy_Y;
    float _par2_linedy_U;
    float _par2_linedy_V;
    float _par2_linedy_Y;
    float* _par_linedy;

    float _par1_line_vtx_density_U;
    float _par1_line_vtx_density_V;
    float _par1_line_vtx_density_Y;
    float _par2_line_vtx_density_U;
    float _par2_line_vtx_density_V;
    float _par2_line_vtx_density_Y;
    float* _par_line_vtx_density;

    float _par1_line_vtx_coverage_U;
    float _par1_line_vtx_coverage_V;
    float _par1_line_vtx_coverage_Y;
    float _par2_line_vtx_coverage_U;
    float _par2_line_vtx_coverage_V;
    float _par2_line_vtx_coverage_Y;
    float* _par_line_vtx_coverage;

    float _par1_line_vtx_charge_U;
    float _par1_line_vtx_charge_V;
    float _par1_line_vtx_charge_Y;
    float _par2_line_vtx_charge_U;
    float _par2_line_vtx_charge_V;
    float _par2_line_vtx_charge_Y;
    float* _par_line_vtx_charge;

    float _par1_triangle_height_U;
    float _par1_triangle_height_V;
    float _par1_triangle_height_Y;
    float _par2_triangle_height_U;
    float _par2_triangle_height_V;
    float _par2_triangle_height_Y;
    float* _par_triangle_height;

    float _par1_triangle_emptyarearatio_U;
    float _par1_triangle_emptyarearatio_V;
    float _par1_triangle_emptyarearatio_Y;
    float _par2_triangle_emptyarearatio_U;
    float _par2_triangle_emptyarearatio_V;
    float _par2_triangle_emptyarearatio_Y;
    float* _par_triangle_emptyarearatio;

    float _par1_triangle_emptyarea_U;
    float _par1_triangle_emptyarea_V;
    float _par1_triangle_emptyarea_Y;
    float _par2_triangle_emptyarea_U;
    float _par2_triangle_emptyarea_V;
    float _par2_triangle_emptyarea_Y;
    float* _par_triangle_emptyarea;

    float _par1_triangle_baselength_U;
    float _par1_triangle_baselength_V;
    float _par1_triangle_baselength_Y;
    float _par2_triangle_baselength_U;
    float _par2_triangle_baselength_V;
    float _par2_triangle_baselength_Y;
    float* _par_triangle_baselength;

    float _par1_triangle_area_U;
    float _par1_triangle_area_V;
    float _par1_triangle_area_Y;
    float _par2_triangle_area_U;
    float _par2_triangle_area_V;
    float _par2_triangle_area_Y;
    float* _par_triangle_area;

    float _par1_triangle_brem_U;
    float _par1_triangle_brem_V;
    float _par1_triangle_brem_Y;
    float _par2_triangle_brem_U;
    float _par2_triangle_brem_V;
    float _par2_triangle_brem_Y;
    float* _par_triangle_brem;

    float _par1_triangle_coverage_U;
    float _par1_triangle_coverage_V;
    float _par1_triangle_coverage_Y;
    float _par2_triangle_coverage_U;
    float _par2_triangle_coverage_V;
    float _par2_triangle_coverage_Y;
    float* _par_triangle_coverage;

    float _par1_expand_charge_U;
    float _par1_expand_charge_V;
    float _par1_expand_charge_Y;
    float _par2_expand_charge_U;
    float _par2_expand_charge_V;
    float _par2_expand_charge_Y;
    float *_par_expand_charge;

    float _par1_dqdx_U;
    float _par1_dqdx_V;
    float _par1_dqdx_Y;
    float _par2_dqdx_U;
    float _par2_dqdx_V;
    float _par2_dqdx_Y;
    float *_par_dqdx;

    int _par1_brem_idx_U;
    int _par1_brem_idx_V;
    int _par1_brem_idx_Y;
    int _par2_brem_idx_U;
    int _par2_brem_idx_V;
    int _par2_brem_idx_Y;
    int *_par_brem_idx;

    float _par1_length3d_U;
    float _par1_length3d_V;
    float _par1_length3d_Y;
    float _par2_length3d_U;
    float _par2_length3d_V;
    float _par2_length3d_Y;
    float *_par_length3d;

    float _par1_showerfrac_U;
    float _par1_showerfrac_V;
    float _par1_showerfrac_Y;
    float _par2_showerfrac_U;
    float _par2_showerfrac_V;
    float _par2_showerfrac_Y;
    float *_par_showerfrac;

    std::vector<int> _par1_numberdefects_U_v;
    std::vector<int> _par1_numberdefects_V_v;
    std::vector<int> _par1_numberdefects_Y_v;
    std::vector<int> _par2_numberdefects_U_v;
    std::vector<int> _par2_numberdefects_V_v;
    std::vector<int> _par2_numberdefects_Y_v;
    std::vector<int>* _par_numberdefects_v;

    std::vector<int> _par1_numberdefects_ns_U_v;
    std::vector<int> _par1_numberdefects_ns_V_v;
    std::vector<int> _par1_numberdefects_ns_Y_v;
    std::vector<int> _par2_numberdefects_ns_U_v;
    std::vector<int> _par2_numberdefects_ns_V_v;
    std::vector<int> _par2_numberdefects_ns_Y_v;
    std::vector<int>* _par_numberdefects_ns_v;
    
    std::vector<float> _par1_largestdefect_U_v;
    std::vector<float> _par1_largestdefect_V_v;
    std::vector<float> _par1_largestdefect_Y_v;
    std::vector<float> _par2_largestdefect_U_v;
    std::vector<float> _par2_largestdefect_V_v;
    std::vector<float> _par2_largestdefect_Y_v;
    std::vector<float>* _par_largestdefect_v;

    std::vector<float> _par1_smallestdefect_U_v;
    std::vector<float> _par1_smallestdefect_V_v;
    std::vector<float> _par1_smallestdefect_Y_v;
    std::vector<float> _par2_smallestdefect_U_v;
    std::vector<float> _par2_smallestdefect_V_v;
    std::vector<float> _par2_smallestdefect_Y_v;
    std::vector<float>* _par_smallestdefect_v;

    std::vector<float> _par1_largestdefect_ns_U_v;
    std::vector<float> _par1_largestdefect_ns_V_v;
    std::vector<float> _par1_largestdefect_ns_Y_v;
    std::vector<float> _par2_largestdefect_ns_U_v;
    std::vector<float> _par2_largestdefect_ns_V_v;
    std::vector<float> _par2_largestdefect_ns_Y_v;
    std::vector<float>* _par_largestdefect_ns_v;

    std::vector<float> _par1_smallestdefect_ns_U_v;
    std::vector<float> _par1_smallestdefect_ns_V_v;
    std::vector<float> _par1_smallestdefect_ns_Y_v;
    std::vector<float> _par2_smallestdefect_ns_U_v;
    std::vector<float> _par2_smallestdefect_ns_V_v;
    std::vector<float> _par2_smallestdefect_ns_Y_v;
    std::vector<float>* _par_smallestdefect_ns_v;

    std::vector<float> _par1_emptyarearatio_U_v;
    std::vector<float> _par1_emptyarearatio_V_v;
    std::vector<float> _par1_emptyarearatio_Y_v;
    std::vector<float> _par2_emptyarearatio_U_v;
    std::vector<float> _par2_emptyarearatio_V_v;
    std::vector<float> _par2_emptyarearatio_Y_v;
    std::vector<float>* _par_emptyarearatio_v;

    std::vector<float> _par1_emptyarea_U_v;
    std::vector<float> _par1_emptyarea_V_v;
    std::vector<float> _par1_emptyarea_Y_v;
    std::vector<float> _par2_emptyarea_U_v;
    std::vector<float> _par2_emptyarea_V_v;
    std::vector<float> _par2_emptyarea_Y_v;
    std::vector<float>* _par_emptyarea_v;

    std::vector<float> _par1_pocketarea_U_v;
    std::vector<float> _par1_pocketarea_V_v;
    std::vector<float> _par1_pocketarea_Y_v;
    std::vector<float> _par2_pocketarea_U_v;
    std::vector<float> _par2_pocketarea_V_v;
    std::vector<float> _par2_pocketarea_Y_v;
    std::vector<float>* _par_pocketarea_v;

    std::vector<float> _par1_pocketarea_ns_U_v;
    std::vector<float> _par1_pocketarea_ns_V_v;
    std::vector<float> _par1_pocketarea_ns_Y_v;
    std::vector<float> _par2_pocketarea_ns_U_v;
    std::vector<float> _par2_pocketarea_ns_V_v;
    std::vector<float> _par2_pocketarea_ns_Y_v;
    std::vector<float>* _par_pocketarea_ns_v;

    std::vector<float> _par1_polyarea_U_v;
    std::vector<float> _par1_polyarea_V_v;
    std::vector<float> _par1_polyarea_Y_v;
    std::vector<float> _par2_polyarea_U_v;
    std::vector<float> _par2_polyarea_V_v;
    std::vector<float> _par2_polyarea_Y_v;
    std::vector<float>* _par_polyarea_v;

    std::vector<float> _par1_polyperimeter_U_v;
    std::vector<float> _par1_polyperimeter_V_v;
    std::vector<float> _par1_polyperimeter_Y_v;
    std::vector<float> _par2_polyperimeter_U_v;
    std::vector<float> _par2_polyperimeter_V_v;
    std::vector<float> _par2_polyperimeter_Y_v;
    std::vector<float>* _par_polyperimeter_v;

    std::vector<float> _par1_polycharge_U_v;
    std::vector<float> _par1_polycharge_V_v;
    std::vector<float> _par1_polycharge_Y_v;
    std::vector<float> _par2_polycharge_U_v;
    std::vector<float> _par2_polycharge_V_v;
    std::vector<float> _par2_polycharge_Y_v;
    std::vector<float>* _par_polycharge_v;
    
    std::vector<int> _par1_polyedges_U_v;
    std::vector<int> _par1_polyedges_V_v;
    std::vector<int> _par1_polyedges_Y_v;
    std::vector<int> _par2_polyedges_U_v;
    std::vector<int> _par2_polyedges_V_v;
    std::vector<int> _par2_polyedges_Y_v;
    std::vector<int>* _par_polyedges_v;

    std::vector<int> _par1_polybranches_U_v;
    std::vector<int> _par1_polybranches_V_v;
    std::vector<int> _par1_polybranches_Y_v;
    std::vector<int> _par2_polybranches_U_v;
    std::vector<int> _par2_polybranches_V_v;
    std::vector<int> _par2_polybranches_Y_v;
    std::vector<int>* _par_polybranches_v;

    std::vector<float> _par1_showerfrac_U_v;
    std::vector<float> _par1_showerfrac_V_v;
    std::vector<float> _par1_showerfrac_Y_v;
    std::vector<float> _par2_showerfrac_U_v;
    std::vector<float> _par2_showerfrac_V_v;
    std::vector<float> _par2_showerfrac_Y_v;
    std::vector<float>* _par_showerfrac_v;

    float _par1_electron_frac_U;
    float _par1_electron_frac_V;
    float _par1_electron_frac_Y;
    float _par2_electron_frac_U;
    float _par2_electron_frac_V;
    float _par2_electron_frac_Y;
    float* _par_electron_frac;

    float _par1_muon_frac_U;
    float _par1_muon_frac_V;
    float _par1_muon_frac_Y;
    float _par2_muon_frac_U;
    float _par2_muon_frac_V;
    float _par2_muon_frac_Y;
    float* _par_muon_frac;

    float _par1_proton_frac_U;
    float _par1_proton_frac_V;
    float _par1_proton_frac_Y;
    float _par2_proton_frac_U;
    float _par2_proton_frac_V;
    float _par2_proton_frac_Y;
    float* _par_proton_frac;

  };

}


#endif
