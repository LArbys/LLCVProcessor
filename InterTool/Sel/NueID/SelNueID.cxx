#ifndef __SELNUEID_CXX__
#define __SELNUEID_CXX__

#include "SelNueID.h"

#include "InterTool_Util/InterImageUtils.h"

#include <iomanip>
#include <sstream>
#include <cstdlib>

#include "Geo2D/Core/LineSegment.h"

#include "LArUtil/GeometryHelper.h"
#include "Base/DataFormatConstants.h"

#include "TVector2.h"

namespace llcv {

  void SelNueID::Configure (const larcv::PSet &pset) {
    set_verbosity((msg::Level_t)pset.get<int>("Verbosity",2));
    LLCV_DEBUG() << "start" << std::endl;
    _cropx = 400;
    _cropy = 400;

    _extension_cutoff = pset.get<float>("ExtensionFraction",0.8);
    _Match.Configure(pset.get<larcv::PSet>("MatchIOU"));

    _twatch.Stop();
  
    _CosmicTag_v.clear();
    _CosmicTag_v.resize(3);

    _brem_dist = pset.get<float>("BremDistance",50);
    _brem_size = pset.get<int>("BremSize",6);

    LLCV_DEBUG() << "end" << std::endl;
  }

  void SelNueID::Initialize() {
    _outtree = new TTree("SelNueID","");
    AttachRSEV(_outtree);
    
    _outtree->Branch("vertex_x", &_vertex_x, "vertex_x/F");
    _outtree->Branch("vertex_y", &_vertex_y, "vertex_y/F");
    _outtree->Branch("vertex_z", &_vertex_z, "vertex_z/F");

    //
    // vertex information
    //
    _outtree->Branch("n_par"                 , &_n_par    , "n_par/I");
    _outtree->Branch("edge_dist_v"           , &_edge_dist_v);
    _outtree->Branch("edge_n_cosmic_v"       , &_edge_n_cosmic_v);
    _outtree->Branch("edge_cosmic_vtx_dist_v", &_edge_cosmic_vtx_dist_v);
    
    //
    // 3D information
    //
    _outtree->Branch("par1_theta"    , &_par1_theta  , "par1_theta/F");
    _outtree->Branch("par1_phi"      , &_par1_phi    , "par1_phi/F");
    _outtree->Branch("par1_length"   , &_par1_length , "par1_length/F");
    _outtree->Branch("par1_score"    , &_par1_score  , "par1_score/F");
    _outtree->Branch("par1_dx"       , &_par1_dx     , "par1_dx/F");
    _outtree->Branch("par1_dy"       , &_par1_dy     , "par1_dy/F");
    _outtree->Branch("par1_dz"       , &_par1_dz     , "par1_dz/F");
    _outtree->Branch("par1_nplanes"  , &_par1_nplanes, "par1_nplanes/I");
    _outtree->Branch("par1_planes_v" , &_par1_planes_v);
    _outtree->Branch("par1_xdead_v"  , &_par1_xdead_v);
    _outtree->Branch("par1_cosmic_dist_v", &_par1_cosmic_dist_v);

    _outtree->Branch("par2_theta"    , &_par2_theta  , "par2_theta/F");
    _outtree->Branch("par2_phi"      , &_par2_phi    , "par2_phi/F");
    _outtree->Branch("par2_length"   , &_par2_length , "par2_length/F");
    _outtree->Branch("par2_score"    , &_par2_score  , "par2_score/F");
    _outtree->Branch("par2_dx"       , &_par2_dx     , "par2_dx/F");
    _outtree->Branch("par2_dy"       , &_par2_dy     , "par2_dy/F");
    _outtree->Branch("par2_dz"       , &_par2_dz     , "par2_dz/F");
    _outtree->Branch("par2_nplanes"  , &_par2_nplanes, "par2_nplanes/I");
    _outtree->Branch("par2_planes_v" , &_par2_planes_v);
    _outtree->Branch("par2_xdead_v"  , &_par2_xdead_v);
    _outtree->Branch("par2_cosmic_dist_v", &_par2_cosmic_dist_v);
    

    //
    // 2D information
    //
    _outtree->Branch("par1_n_polygons_U", &_par1_n_polygons_U, "par1_n_polygons_U/I");
    _outtree->Branch("par1_n_polygons_V", &_par1_n_polygons_V, "par1_n_polygons_V/I");
    _outtree->Branch("par1_n_polygons_Y", &_par1_n_polygons_Y, "par1_n_polygons_Y/I");
    _outtree->Branch("par2_n_polygons_U", &_par2_n_polygons_U, "par2_n_polygons_U/I");
    _outtree->Branch("par2_n_polygons_V", &_par2_n_polygons_V, "par2_n_polygons_V/I");
    _outtree->Branch("par2_n_polygons_Y", &_par2_n_polygons_Y, "par2_n_polygons_Y/I");

    _outtree->Branch("par1_linelength_U", &_par1_linelength_U, "par1_linelength_U/F");
    _outtree->Branch("par1_linelength_V", &_par1_linelength_V, "par1_linelength_V/F");
    _outtree->Branch("par1_linelength_Y", &_par1_linelength_Y, "par1_linelength_Y/F");
    _outtree->Branch("par2_linelength_U", &_par2_linelength_U, "par2_linelength_U/F");
    _outtree->Branch("par2_linelength_V", &_par2_linelength_V, "par2_linelength_V/F");
    _outtree->Branch("par2_linelength_Y", &_par2_linelength_Y, "par2_linelength_Y/F");

    _outtree->Branch("par1_linefrac_U", &_par1_linefrac_U, "par1_linefrac_U/F");
    _outtree->Branch("par1_linefrac_V", &_par1_linefrac_V, "par1_linefrac_V/F");
    _outtree->Branch("par1_linefrac_Y", &_par1_linefrac_Y, "par1_linefrac_Y/F");
    _outtree->Branch("par2_linefrac_U", &_par2_linefrac_U, "par2_linefrac_U/F");
    _outtree->Branch("par2_linefrac_V", &_par2_linefrac_V, "par2_linefrac_V/F");
    _outtree->Branch("par2_linefrac_Y", &_par2_linefrac_Y, "par2_linefrac_Y/F");

    _outtree->Branch("par1_linedx_U", &_par1_linedx_U, "par1_linedx_U/F");
    _outtree->Branch("par1_linedx_V", &_par1_linedx_V, "par1_linedx_V/F");
    _outtree->Branch("par1_linedx_Y", &_par1_linedx_Y, "par1_linedx_Y/F");
    _outtree->Branch("par2_linedx_U", &_par2_linedx_U, "par2_linedx_U/F");
    _outtree->Branch("par2_linedx_V", &_par2_linedx_V, "par2_linedx_V/F");
    _outtree->Branch("par2_linedx_Y", &_par2_linedx_Y, "par2_linedx_Y/F");
    
    _outtree->Branch("par1_linedy_U", &_par1_linedy_U, "par1_linedy_U/F");
    _outtree->Branch("par1_linedy_V", &_par1_linedy_V, "par1_linedy_V/F");
    _outtree->Branch("par1_linedy_Y", &_par1_linedy_Y, "par1_linedy_Y/F");
    _outtree->Branch("par2_linedy_U", &_par2_linedy_U, "par2_linedy_U/F");
    _outtree->Branch("par2_linedy_V", &_par2_linedy_V, "par2_linedy_V/F");
    _outtree->Branch("par2_linedy_Y", &_par2_linedy_Y, "par2_linedy_Y/F");

    _outtree->Branch("par1_triangle_height_U", &_par1_triangle_height_U, "par1_triangle_height_U/F");
    _outtree->Branch("par1_triangle_height_V", &_par1_triangle_height_V, "par1_triangle_height_V/F");
    _outtree->Branch("par1_triangle_height_Y", &_par1_triangle_height_Y, "par1_triangle_height_Y/F");
    _outtree->Branch("par2_triangle_height_U", &_par2_triangle_height_U, "par2_triangle_height_U/F");
    _outtree->Branch("par2_triangle_height_V", &_par2_triangle_height_V, "par2_triangle_height_V/F");
    _outtree->Branch("par2_triangle_height_Y", &_par2_triangle_height_Y, "par2_triangle_height_Y/F");

    _outtree->Branch("par1_triangle_emptyarearatio_U", &_par1_triangle_emptyarearatio_U, "par1_triangle_emptyarearatio_U/F");
    _outtree->Branch("par1_triangle_emptyarearatio_V", &_par1_triangle_emptyarearatio_V, "par1_triangle_emptyarearatio_V/F");
    _outtree->Branch("par1_triangle_emptyarearatio_Y", &_par1_triangle_emptyarearatio_Y, "par1_triangle_emptyarearatio_Y/F");
    _outtree->Branch("par2_triangle_emptyarearatio_U", &_par2_triangle_emptyarearatio_U, "par2_triangle_emptyarearatio_U/F");
    _outtree->Branch("par2_triangle_emptyarearatio_V", &_par2_triangle_emptyarearatio_V, "par2_triangle_emptyarearatio_V/F");
    _outtree->Branch("par2_triangle_emptyarearatio_Y", &_par2_triangle_emptyarearatio_Y, "par2_triangle_emptyarearatio_Y/F");

    _outtree->Branch("par1_triangle_emptyarea_U", &_par1_triangle_emptyarea_U, "par1_triangle_emptyarea_U/F");
    _outtree->Branch("par1_triangle_emptyarea_V", &_par1_triangle_emptyarea_V, "par1_triangle_emptyarea_V/F");
    _outtree->Branch("par1_triangle_emptyarea_Y", &_par1_triangle_emptyarea_Y, "par1_triangle_emptyarea_Y/F");
    _outtree->Branch("par2_triangle_emptyarea_U", &_par2_triangle_emptyarea_U, "par2_triangle_emptyarea_U/F");
    _outtree->Branch("par2_triangle_emptyarea_V", &_par2_triangle_emptyarea_V, "par2_triangle_emptyarea_V/F");
    _outtree->Branch("par2_triangle_emptyarea_Y", &_par2_triangle_emptyarea_Y, "par2_triangle_emptyarea_Y/F");

    _outtree->Branch("par1_triangle_baselength_U", &_par1_triangle_baselength_U, "par1_triangle_baselength_U/F");
    _outtree->Branch("par1_triangle_baselength_V", &_par1_triangle_baselength_V, "par1_triangle_baselength_V/F");
    _outtree->Branch("par1_triangle_baselength_Y", &_par1_triangle_baselength_Y, "par1_triangle_baselength_Y/F");
    _outtree->Branch("par2_triangle_baselength_U", &_par2_triangle_baselength_U, "par2_triangle_baselength_U/F");
    _outtree->Branch("par2_triangle_baselength_V", &_par2_triangle_baselength_V, "par2_triangle_baselength_V/F");
    _outtree->Branch("par2_triangle_baselength_Y", &_par2_triangle_baselength_Y, "par2_triangle_baselength_Y/F");

    _outtree->Branch("par1_triangle_area_U", &_par1_triangle_area_U, "par1_triangle_area_U/F");
    _outtree->Branch("par1_triangle_area_V", &_par1_triangle_area_V, "par1_triangle_area_V/F");
    _outtree->Branch("par1_triangle_area_Y", &_par1_triangle_area_Y, "par1_triangle_area_Y/F");
    _outtree->Branch("par2_triangle_area_U", &_par2_triangle_area_U, "par2_triangle_area_U/F");
    _outtree->Branch("par2_triangle_area_V", &_par2_triangle_area_V, "par2_triangle_area_V/F");
    _outtree->Branch("par2_triangle_area_Y", &_par2_triangle_area_Y, "par2_triangle_area_Y/F");

    _outtree->Branch("par1_triangle_brem_U", &_par1_triangle_brem_U, "par1_triangle_brem_U/F");
    _outtree->Branch("par1_triangle_brem_V", &_par1_triangle_brem_V, "par1_triangle_brem_V/F");
    _outtree->Branch("par1_triangle_brem_Y", &_par1_triangle_brem_Y, "par1_triangle_brem_Y/F");
    _outtree->Branch("par2_triangle_brem_U", &_par2_triangle_brem_U, "par2_triangle_brem_U/F");
    _outtree->Branch("par2_triangle_brem_V", &_par2_triangle_brem_V, "par2_triangle_brem_V/F");
    _outtree->Branch("par2_triangle_brem_Y", &_par2_triangle_brem_Y, "par2_triangle_brem_Y/F");

    _outtree->Branch("par1_triangle_coverage_U", &_par1_triangle_coverage_U, "par1_triangle_coverage_U/F");
    _outtree->Branch("par1_triangle_coverage_V", &_par1_triangle_coverage_V, "par1_triangle_coverage_V/F");
    _outtree->Branch("par1_triangle_coverage_Y", &_par1_triangle_coverage_Y, "par1_triangle_coverage_Y/F");
    _outtree->Branch("par2_triangle_coverage_U", &_par2_triangle_coverage_U, "par2_triangle_coverage_U/F");
    _outtree->Branch("par2_triangle_coverage_V", &_par2_triangle_coverage_V, "par2_triangle_coverage_V/F");
    _outtree->Branch("par2_triangle_coverage_Y", &_par2_triangle_coverage_Y, "par2_triangle_coverage_Y/F");

    _outtree->Branch("par1_brem_idx_U", &_par1_brem_idx_U, "par1_brem_idx_U/I");
    _outtree->Branch("par1_brem_idx_V", &_par1_brem_idx_V, "par1_brem_idx_V/I");
    _outtree->Branch("par1_brem_idx_Y", &_par1_brem_idx_Y, "par1_brem_idx_Y/I");
    _outtree->Branch("par2_brem_idx_U", &_par2_brem_idx_U, "par2_brem_idx_U/I");
    _outtree->Branch("par2_brem_idx_V", &_par2_brem_idx_V, "par2_brem_idx_V/I");
    _outtree->Branch("par2_brem_idx_Y", &_par2_brem_idx_Y, "par2_brem_idx_Y/I");

    _outtree->Branch("par1_expand_charge_U", &_par1_expand_charge_U, "par1_expand_charge_U/F");
    _outtree->Branch("par1_expand_charge_V", &_par1_expand_charge_V, "par1_expand_charge_V/F");
    _outtree->Branch("par1_expand_charge_Y", &_par1_expand_charge_Y, "par1_expand_charge_Y/F");
    _outtree->Branch("par2_expand_charge_U", &_par2_expand_charge_U, "par2_expand_charge_U/F");
    _outtree->Branch("par2_expand_charge_V", &_par2_expand_charge_V, "par2_expand_charge_V/F");
    _outtree->Branch("par2_expand_charge_Y", &_par2_expand_charge_Y, "par2_expand_charge_Y/F");

    _outtree->Branch("par1_length3d_U", &_par1_length3d_U, "par1_length3d_U/F");
    _outtree->Branch("par1_length3d_V", &_par1_length3d_V, "par1_length3d_V/F");
    _outtree->Branch("par1_length3d_Y", &_par1_length3d_Y, "par1_length3d_Y/F");
    _outtree->Branch("par2_length3d_U", &_par2_length3d_U, "par2_length3d_U/F");
    _outtree->Branch("par2_length3d_V", &_par2_length3d_V, "par2_length3d_V/F");
    _outtree->Branch("par2_length3d_Y", &_par2_length3d_Y, "par2_length3d_Y/F");

    _outtree->Branch("par1_showerfrac_U", &_par1_showerfrac_U, "par1_showerfrac_U/F");
    _outtree->Branch("par1_showerfrac_V", &_par1_showerfrac_V, "par1_showerfrac_V/F");
    _outtree->Branch("par1_showerfrac_Y", &_par1_showerfrac_Y, "par1_showerfrac_Y/F");
    _outtree->Branch("par2_showerfrac_U", &_par2_showerfrac_U, "par2_showerfrac_U/F");
    _outtree->Branch("par2_showerfrac_V", &_par2_showerfrac_V, "par2_showerfrac_V/F");
    _outtree->Branch("par2_showerfrac_Y", &_par2_showerfrac_Y, "par2_showerfrac_Y/F");
    
    _outtree->Branch("par1_numberdefects_U_v", &_par1_numberdefects_U_v);
    _outtree->Branch("par1_numberdefects_V_v", &_par1_numberdefects_V_v);
    _outtree->Branch("par1_numberdefects_Y_v", &_par1_numberdefects_Y_v);
    _outtree->Branch("par2_numberdefects_U_v", &_par2_numberdefects_U_v);
    _outtree->Branch("par2_numberdefects_V_v", &_par2_numberdefects_V_v);
    _outtree->Branch("par2_numberdefects_Y_v", &_par2_numberdefects_Y_v);
    
    _outtree->Branch("par1_numberdefects_ns_U_v", &_par1_numberdefects_ns_U_v);
    _outtree->Branch("par1_numberdefects_ns_V_v", &_par1_numberdefects_ns_V_v);
    _outtree->Branch("par1_numberdefects_ns_Y_v", &_par1_numberdefects_ns_Y_v);
    _outtree->Branch("par2_numberdefects_ns_U_v", &_par2_numberdefects_ns_U_v);
    _outtree->Branch("par2_numberdefects_ns_V_v", &_par2_numberdefects_ns_V_v);
    _outtree->Branch("par2_numberdefects_ns_Y_v", &_par2_numberdefects_ns_Y_v);
    
    _outtree->Branch("par1_largestdefect_U_v", &_par1_largestdefect_U_v);
    _outtree->Branch("par1_largestdefect_V_v", &_par1_largestdefect_V_v);
    _outtree->Branch("par1_largestdefect_Y_v", &_par1_largestdefect_Y_v);
    _outtree->Branch("par2_largestdefect_U_v", &_par2_largestdefect_U_v);
    _outtree->Branch("par2_largestdefect_V_v", &_par2_largestdefect_V_v);
    _outtree->Branch("par2_largestdefect_Y_v", &_par2_largestdefect_Y_v);

    _outtree->Branch("par1_smallestdefect_U_v", &_par1_smallestdefect_U_v);
    _outtree->Branch("par1_smallestdefect_V_v", &_par1_smallestdefect_V_v);

    _outtree->Branch("par2_largestdefect_ns_V_v", &_par2_largestdefect_ns_V_v);

    _outtree->Branch("par2_smallestdefect_ns_U_v", &_par2_smallestdefect_ns_U_v);    _outtree->Branch("par1_emptyarearatio_V_v", &_par1_emptyarearatio_V_v);

    _outtree->Branch("par1_emptyarea_U_v", &_par1_emptyarea_U_v);

    _outtree->Branch("par1_smallestdefect_V_v", &_par1_smallestdefect_V_v);

    _outtree->Branch("par2_largestdefect_ns_V_v", &_par2_largestdefect_ns_V_v);

    _outtree->Branch("par2_smallestdefect_ns_U_v", &_par2_smallestdefect_ns_U_v);    _outtree->Branch("par1_emptyarearatio_V_v", &_par1_emptyarearatio_V_v);

    _outtree->Branch("par1_emptyarea_U_v", &_par1_emptyarea_U_v);    _outtree->Branch("par1_smallestdefect_Y_v", &_par1_smallestdefect_Y_v);
    _outtree->Branch("par1_largestdefect_ns_U_v", &_par1_largestdefect_ns_U_v);
    _outtree->Branch("par2_largestdefect_ns_Y_v", &_par2_largestdefect_ns_Y_v);    _outtree->Branch("par1_smallestdefect_ns_Y_v", &_par1_smallestdefect_ns_Y_v);

    _outtree->Branch("par1_emptyarearatio_Y_v", &_par1_emptyarearatio_Y_v);



    _outtree->Branch("par1_smallestdefect_Y_v", &_par1_smallestdefect_Y_v);
    _outtree->Branch("par1_largestdefect_ns_U_v", &_par1_largestdefect_ns_U_v);
    _outtree->Branch("par2_largestdefect_ns_Y_v", &_par2_largestdefect_ns_Y_v);    _outtree->Branch("par1_smallestdefect_ns_Y_v", &_par1_smallestdefect_ns_Y_v);

    _outtree->Branch("par1_emptyarearatio_Y_v", &_par1_emptyarearatio_Y_v);

    _outtree->Branch("par2_smallestdefect_U_v", &_par2_smallestdefect_U_v);

    _outtree->Branch("par2_largestdefect_ns_U_v", &_par2_largestdefect_ns_U_v);

    _outtree->Branch("par2_smallestdefect_ns_V_v", &_par2_smallestdefect_ns_V_v);    _outtree->Branch("par1_emptyarearatio_U_v", &_par1_emptyarearatio_U_v);
    _outtree->Branch("par2_emptyarearatio_Y_v", &_par2_emptyarearatio_Y_v);    _outtree->Branch("par1_emptyarea_V_v", &_par1_emptyarea_V_v);

    _outtree->Branch("par2_smallestdefect_U_v", &_par2_smallestdefect_U_v);

    _outtree->Branch("par2_largestdefect_ns_U_v", &_par2_largestdefect_ns_U_v);

    _outtree->Branch("par2_smallestdefect_ns_V_v", &_par2_smallestdefect_ns_V_v);    _outtree->Branch("par1_emptyarearatio_U_v", &_par1_emptyarearatio_U_v);
    _outtree->Branch("par2_emptyarearatio_Y_v", &_par2_emptyarearatio_Y_v);    _outtree->Branch("par1_emptyarea_V_v", &_par1_emptyarea_V_v);    _outtree->Branch("par2_smallestdefect_V_v", &_par2_smallestdefect_V_v);
    _outtree->Branch("par1_largestdefect_ns_V_v", &_par1_largestdefect_ns_V_v);
    
    _outtree->Branch("par1_smallestdefect_ns_V_v", &_par1_smallestdefect_ns_V_v);

    _outtree->Branch("par2_emptyarearatio_U_v", &_par2_emptyarearatio_U_v);



    _outtree->Branch("par2_smallestdefect_V_v", &_par2_smallestdefect_V_v);
    _outtree->Branch("par1_largestdefect_ns_V_v", &_par1_largestdefect_ns_V_v);
    
    _outtree->Branch("par1_smallestdefect_ns_V_v", &_par1_smallestdefect_ns_V_v);

    _outtree->Branch("par2_emptyarearatio_U_v", &_par2_emptyarearatio_U_v);

    _outtree->Branch("par2_smallestdefect_Y_v", &_par2_smallestdefect_Y_v);

    _outtree->Branch("par1_largestdefect_ns_Y_v", &_par1_largestdefect_ns_Y_v);    _outtree->Branch("par1_smallestdefect_ns_U_v", &_par1_smallestdefect_ns_U_v);
    _outtree->Branch("par2_smallestdefect_ns_Y_v", &_par2_smallestdefect_ns_Y_v); 

    _outtree->Branch("par2_emptyarearatio_V_v", &_par2_emptyarearatio_V_v);    _outtree->Branch("par1_emptyarea_Y_v", &_par1_emptyarea_Y_v);

    _outtree->Branch("par2_smallestdefect_Y_v", &_par2_smallestdefect_Y_v);

    _outtree->Branch("par1_largestdefect_ns_Y_v", &_par1_largestdefect_ns_Y_v);    _outtree->Branch("par1_smallestdefect_ns_U_v", &_par1_smallestdefect_ns_U_v);
    _outtree->Branch("par2_smallestdefect_ns_Y_v", &_par2_smallestdefect_ns_Y_v); 

    _outtree->Branch("par2_emptyarearatio_V_v", &_par2_emptyarearatio_V_v);    _outtree->Branch("par1_emptyarea_Y_v", &_par1_emptyarea_Y_v);
    _outtree->Branch("par2_emptyarea_U_v", &_par2_emptyarea_U_v);
    _outtree->Branch("par2_emptyarea_V_v", &_par2_emptyarea_V_v);
    _outtree->Branch("par2_emptyarea_Y_v", &_par2_emptyarea_Y_v);

    _outtree->Branch("par1_pocketarea_U_v", &_par1_pocketarea_U_v);
    _outtree->Branch("par1_pocketarea_V_v", &_par1_pocketarea_V_v);
    _outtree->Branch("par1_pocketarea_Y_v", &_par1_pocketarea_Y_v);
    _outtree->Branch("par2_pocketarea_U_v", &_par2_pocketarea_U_v);
    _outtree->Branch("par2_pocketarea_V_v", &_par2_pocketarea_V_v);
    _outtree->Branch("par2_pocketarea_Y_v", &_par2_pocketarea_Y_v);

    _outtree->Branch("par1_pocketarea_ns_U_v", &_par1_pocketarea_ns_U_v);
    _outtree->Branch("par1_pocketarea_ns_V_v", &_par1_pocketarea_ns_V_v);
    _outtree->Branch("par1_pocketarea_ns_Y_v", &_par1_pocketarea_ns_Y_v);
    _outtree->Branch("par2_pocketarea_ns_U_v", &_par2_pocketarea_ns_U_v);
    _outtree->Branch("par2_pocketarea_ns_V_v", &_par2_pocketarea_ns_V_v);
    _outtree->Branch("par2_pocketarea_ns_Y_v", &_par2_pocketarea_ns_Y_v);

    _outtree->Branch("par1_poly_area_U_v", &_par1_polyarea_U_v);
    _outtree->Branch("par1_poly_area_V_v", &_par1_polyarea_V_v);
    _outtree->Branch("par1_poly_area_Y_v", &_par1_polyarea_Y_v);
    _outtree->Branch("par2_poly_area_U_v", &_par2_polyarea_U_v);
    _outtree->Branch("par2_poly_area_V_v", &_par2_polyarea_V_v);
    _outtree->Branch("par2_poly_area_Y_v", &_par2_polyarea_Y_v);

    _outtree->Branch("par1_polyperimeter_U_v", &_par1_polyperimeter_U_v);
    _outtree->Branch("par1_polyperimeter_V_v", &_par1_polyperimeter_V_v);
    _outtree->Branch("par1_polyperimeter_Y_v", &_par1_polyperimeter_Y_v);
    _outtree->Branch("par2_polyperimeter_U_v", &_par2_polyperimeter_U_v);
    _outtree->Branch("par2_polyperimeter_V_v", &_par2_polyperimeter_V_v);
    _outtree->Branch("par2_polyperimeter_Y_v", &_par2_polyperimeter_Y_v);

    _outtree->Branch("par1_polycharge_U_v", &_par1_polycharge_U_v);
    _outtree->Branch("par1_polycharge_V_v", &_par1_polycharge_V_v);
    _outtree->Branch("par1_polycharge_Y_v", &_par1_polycharge_Y_v);
    _outtree->Branch("par2_polycharge_U_v", &_par2_polycharge_U_v);
    _outtree->Branch("par2_polycharge_V_v", &_par2_polycharge_V_v);
    _outtree->Branch("par2_polycharge_Y_v", &_par2_polycharge_Y_v);

    _outtree->Branch("par1_polyedges_U_v", &_par1_polyedges_U_v);
    _outtree->Branch("par1_polyedges_V_v", &_par1_polyedges_V_v);
    _outtree->Branch("par1_polyedges_Y_v", &_par1_polyedges_Y_v);
    _outtree->Branch("par2_polyedges_U_v", &_par2_polyedges_U_v);
    _outtree->Branch("par2_polyedges_V_v", &_par2_polyedges_V_v);
    _outtree->Branch("par2_polyedges_Y_v", &_par2_polyedges_Y_v);

    _outtree->Branch("par1_polybranches_U_v", &_par1_polybranches_U_v);
    _outtree->Branch("par1_polybranches_V_v", &_par1_polybranches_V_v);
    _outtree->Branch("par1_polybranches_Y_v", &_par1_polybranches_Y_v);
    _outtree->Branch("par2_polybranches_U_v", &_par2_polybranches_U_v);
    _outtree->Branch("par2_polybranches_V_v", &_par2_polybranches_V_v);
    _outtree->Branch("par2_polybranches_Y_v", &_par2_polybranches_Y_v);

    _outtree->Branch("par1_showerfrac_U_v", &_par1_showerfrac_U_v);
    _outtree->Branch("par1_showerfrac_V_v", &_par1_showerfrac_V_v);
    _outtree->Branch("par1_showerfrac_Y_v", &_par1_showerfrac_Y_v);
    _outtree->Branch("par2_showerfrac_U_v", &_par2_showerfrac_U_v);
    _outtree->Branch("par2_showerfrac_V_v", &_par2_showerfrac_V_v);
    _outtree->Branch("par2_showerfrac_Y_v", &_par2_showerfrac_Y_v);
    
    
    //
    // segment information
    //
    _outtree->Branch("par1_electron_frac_U", &_par1_electron_frac_U, "par1_electron_frac_U/F");
    _outtree->Branch("par1_electron_frac_V", &_par1_electron_frac_V, "par1_electron_frac_V/F");
    _outtree->Branch("par1_electron_frac_Y", &_par1_electron_frac_Y, "par1_electron_frac_Y/F");

    _outtree->Branch("par2_electron_frac_U", &_par2_electron_frac_U, "par2_electron_frac_U/F");
    _outtree->Branch("par2_electron_frac_V", &_par2_electron_frac_V, "par2_electron_frac_V/F");
    _outtree->Branch("par2_electron_frac_Y", &_par2_electron_frac_Y, "par2_electron_frac_Y/F");

    _outtree->Branch("par1_muon_frac_U", &_par1_muon_frac_U, "par1_muon_frac_U/F");
    _outtree->Branch("par1_muon_frac_V", &_par1_muon_frac_V, "par1_muon_frac_V/F");
    _outtree->Branch("par1_muon_frac_Y", &_par1_muon_frac_Y, "par1_muon_frac_Y/F");

    _outtree->Branch("par2_muon_frac_U", &_par2_muon_frac_U, "par2_muon_frac_U/F");
    _outtree->Branch("par2_muon_frac_V", &_par2_muon_frac_V, "par2_muon_frac_V/F");
    _outtree->Branch("par2_muon_frac_Y", &_par2_muon_frac_Y, "par2_muon_frac_Y/F");

    _outtree->Branch("par1_proton_frac_U", &_par1_proton_frac_U, "par1_proton_frac_U/F");
    _outtree->Branch("par1_proton_frac_V", &_par1_proton_frac_V, "par1_proton_frac_V/F");
    _outtree->Branch("par1_proton_frac_Y", &_par1_proton_frac_Y, "par1_proton_frac_Y/F");

    _outtree->Branch("par2_proton_frac_U", &_par2_proton_frac_U, "par2_proton_frac_U/F");
    _outtree->Branch("par2_proton_frac_V", &_par2_proton_frac_V, "par2_proton_frac_V/F");
    _outtree->Branch("par2_proton_frac_Y", &_par2_proton_frac_Y, "par2_proton_frac_Y/F");


    return;
  }

  double SelNueID::Select() {
    LLCV_DEBUG() << "start" << std::endl;

    
    LLCV_DEBUG() << "(RSEV)=("<< Run() << "," << SubRun() << "," << Event() << "," << VertexID() << ")" << std::endl;    
    LLCV_DEBUG() << "VTX=(" 
		 << Data().Vertex()->X() << "," 
		 << Data().Vertex()->Y() << "," 
		 << Data().Vertex()->Z() << ")" << std::endl;

    auto mat_v  = Image().Image<cv::Mat>(kImageADC,_cropx,_cropy);
    auto meta_v = Image().Image<larocv::ImageMeta>(kImageADC,_cropx,_cropy);
    auto img_v  = Image().Image<larcv::Image2D>(kImageADC,_cropx,_cropy);
    auto dead_v = Image().Image<cv::Mat>(kImageDead,_cropx,_cropy);
    auto shr_v  = Image().Image<cv::Mat>(kImageShower,_cropx,_cropy);
    
    _white_img = larocv::BlankImage(*(mat_v.front()),0);
    _white_img.setTo(cv::Scalar(0));
    
    _ContourScan.Reset();
    
    for(const auto& meta : meta_v)
      _ContourScan.SetPlaneInfo(*meta);
    
    std::array<cv::Mat,3> aimg_v;   // adc image
    std::array<cv::Mat,3> timg_v;   // threshold image
    std::array<cv::Mat,3> cimg_v;   // cosmic rejected image
    std::array<cv::Mat,3> dimg_v;   // dead image
    std::array<cv::Mat,3> mat3d_v;  // threshold 8UC3 image

    for(size_t plane=0; plane<3; ++plane) {
      aimg_v[plane]   = *(mat_v[plane]);
      timg_v[plane]   = larocv::Threshold(*(mat_v[plane]),10,255);
      mat3d_v[plane]  = As8UC3(timg_v[plane]);
      dimg_v[plane]   = *(dead_v[plane]);
      
      // tag cosmics
      auto& _CosmicTag = _CosmicTag_v[plane];
      _CosmicTag.Reset();
      _CosmicTag.TagCosmic(timg_v[plane],dimg_v[plane]);

      cimg_v[plane] = timg_v[plane].clone();

      for(const auto& ctor : _CosmicTag.CosmicContours())
	cimg_v[plane] = larocv::MaskImage(cimg_v[plane],ctor,-1,true);  
    }
    
    larocv::data::Vertex3D vtx3d;
    vtx3d.x = Data().Vertex()->X();
    vtx3d.y = Data().Vertex()->Y();
    vtx3d.z = Data().Vertex()->Z();

    _vertex_x = (float)vtx3d.x;
    _vertex_y = (float)vtx3d.y;
    _vertex_z = (float)vtx3d.z;

    //
    // Project the vertex onto the plane
    //
    larocv::GEO2D_Contour_t vertex_pt_v;
    vertex_pt_v.resize(3);

    for(size_t plane=0; plane<3; ++plane) {
      int px_x = kINVALID_INT;
      int px_y = kINVALID_INT;

      ProjectMat(img_v[plane]->meta(),
		 vtx3d.x,vtx3d.y,vtx3d.z,
    		 px_x, px_y);

      vertex_pt_v[plane] = cv::Point_<int>(px_y,px_x);
    }

    //
    // Generate initial 2D clusters
    //
    std::array<larocv::GEO2D_ContourArray_t,3> plane_ctor_vv;
    std::array<larocv::GEO2D_Contour_t,3> vertex_ctor_v;
    std::array<size_t,3> close_id_v;

    std::array<larocv::GEO2D_ContourArray_t,3> par_ctor_v;
    std::array<larocv::GEO2D_ContourArray_t,3> line_contour_vv;
    std::array<std::vector<Triangle>,3> triangle_vv;
    std::array<geo2d::VectorArray<float>,3> edge_vv;

    float _distance_tol = 20;

    for(size_t plane=0; plane<3; ++plane) {
      auto& cimg         = cimg_v[plane];
      auto& close_id     = close_id_v[plane];
      auto& vertex_ctor  = vertex_ctor_v[plane];
      auto& plane_ctor_v = plane_ctor_vv[plane];

      plane_ctor_v = larocv::FindContours(cimg);
      
      LLCV_DEBUG() << "@plane=" << plane 
		   << " found " << plane_ctor_v.size() 
		   << " ctors" << std::endl;
      
      const auto vertex_pt = vertex_pt_v[plane];
      
      float distance = larocv::kINVALID_FLOAT;
      close_id = FindClosestContour(plane_ctor_v,vertex_pt,distance);

      if (close_id == larocv::kINVALID_SIZE) {
	LLCV_WARNING() << "No contour? Skip." << std::endl;
	continue;
      }

      if (distance > _distance_tol) {
	LLCV_WARNING() << "Distance=" << distance << ">" << _distance_tol << ". Skip." << std::endl;
	continue;
      }

      vertex_ctor = plane_ctor_v[close_id];
      
      // mask all but vertex_ctor
      auto cimg_vertex_mask = larocv::MaskImage(cimg,vertex_ctor,-1,false);

      // minimize over circle of radius 4
      auto nz_pt_v = larocv::FindNonZero(larocv::MaskImage(cimg_vertex_mask,geo2d::Circle<float>(vertex_pt,4),-1,false));

      float nratio_max = -1.0*larocv::kINVALID_FLOAT;

      for(const auto& nz_pt : nz_pt_v) {
	
	// mask out the vertex from image
	auto cimg_mask = larocv::MaskImage(cimg_vertex_mask,geo2d::Circle<float>(nz_pt,5),-1,true);
      
	// find contours
	auto& par_ctor = par_ctor_v[plane];
	par_ctor = larocv::FindContours(cimg_mask);

	auto nctor = par_ctor.size();

	// number particles == 1 veto
	if (nctor < 2) continue;
	
	larocv::GEO2D_ContourArray_t local_line_contour_v;
	std::vector<Triangle>        local_triangle_v;
	geo2d::VectorArray<float>    local_edge_v;
	std::vector<float>           local_line_frac_v;

	local_line_contour_v.reserve(nctor);
	local_triangle_v.reserve(nctor);
	local_edge_v.reserve(nctor);
	local_line_frac_v.reserve(nctor);

	for(size_t pid=0; pid<(size_t)nctor; ++pid) {

	  auto par = par_ctor[pid];

	  // estimate the end point
	  Triangle local_triangle(par,nz_pt);
	  
	  geo2d::Vector<float> local_edge;
	  float nline_pixels = 0;
	  auto local_line_ctor = MaximizeLine(cimg,local_triangle,nline_pixels,local_edge);
	  
	  if (local_line_ctor.empty()) continue;
	  
	  float npar_pixels = larocv::CountNonZero(larocv::MaskImage(cimg,par,-1,false));

	  float nratio = 0;
	  if (npar_pixels > 0)
	    nratio = nline_pixels / npar_pixels;
	  else
	    continue;
	  
	  local_line_frac_v.emplace_back(nratio);
	  local_line_contour_v.emplace_back(local_line_ctor);
	  local_triangle_v.emplace_back(local_triangle);
	  local_edge_v.emplace_back(local_edge);
	  
	} // end contour

	// number particles == 1 veto
	if (local_line_frac_v.size() < 2) 
	  continue;

	// check ratio product
	float ratio_prod = 1;
	for(auto local_line_frac : local_line_frac_v)
	  ratio_prod *= local_line_frac;
	
	// if it's higher, save
	if (ratio_prod > nratio_max) {
	  line_contour_vv[plane] = local_line_contour_v;
	  triangle_vv[plane]     = local_triangle_v;
	  edge_vv[plane]         = local_edge_v;
	  
	  nratio_max = ratio_prod;
	  LLCV_DEBUG() << "saved r=" << ratio_prod << std::endl;
	}

      } // end nz_pt

    } // end plane

    

    //
    // get the max, min, time for contours
    //
    float tlo = larocv::kINVALID_FLOAT;
    float thi = -1.0*larocv::kINVALID_FLOAT;
    for(size_t plane=0; plane<3; ++plane) {
      for (const auto& edge : edge_vv[plane]) {
	tlo = std::min(tlo,edge.x);
	thi = std::max(thi,edge.x);
      }
      LLCV_DEBUG() << "@plane=" << plane << " tlo=" << tlo << " thi=" << thi << std::endl;
    }

    // per plane, per line, collection of contours
    std::array<std::vector<std::vector<const larocv::GEO2D_Contour_t*> > ,3> ctor_overlap_vvv;

    //
    // extend line to touch potential charge
    // if line is left, go -x
    // if line is right, go +x
    //

    for(size_t plane=0; plane<3; ++plane) {
      const auto& line_contour_v = line_contour_vv[plane];
      const auto& triangle_v     = triangle_vv[plane];
      const auto& edge_v         = edge_vv[plane];

      auto& ctor_overlap_vv = ctor_overlap_vvv[plane];

      const auto& plane_ctor_v = plane_ctor_vv[plane];
      const auto& close_id = close_id_v[plane];
      
      ctor_overlap_vv.resize(line_contour_v.size());

      for(size_t cid=0; cid<line_contour_v.size(); ++cid) {
	auto& ctor_overlap_v = ctor_overlap_vv[cid];
	
	_white_img.setTo(cv::Scalar(0));

	// determine the direction
	const auto& edge = edge_v[cid];
	const auto& triangle = triangle_v[cid];
	
	geo2d::Vector<float> new_edge;
	geo2d::Line<float> line(triangle.Apex(),edge - triangle.Apex());
	
	if (triangle.Apex().x == edge.x)
	  continue;
	
	// extend backwards
	if (edge.x < triangle.Apex().x) {
	  new_edge.x = tlo;
	  new_edge.y = line.y(new_edge.x);
	}
	// extend forwards
	else {
	  new_edge.x = thi;
	  new_edge.y = line.y(new_edge.x);
	}
	
	// Determine if line intersects new charge
	cv::line(_white_img,triangle.Apex(),new_edge,cv::Scalar(255),3);
	auto lc_v = larocv::FindContours(_white_img);
	if (lc_v.empty()) continue;

	const auto& lc = lc_v.front();

	for(size_t ict=0; ict<plane_ctor_v.size(); ++ict) {
	  if (ict == close_id) continue;
	  if (!larocv::AreaOverlap(plane_ctor_v[ict],lc)) continue;
	  ctor_overlap_v.push_back(&(plane_ctor_v[ict]));
	} // end intersection
      } // end this line
    } // end this plane

    //
    // Append new charge, fill out Object2D
    //

    std::array<std::vector<Object2D>,3> object_vv;

    for(size_t plane=0; plane<3; ++plane) {
      const auto& cimg   = cimg_v[plane];

      auto& line_contour_v = line_contour_vv[plane];
      auto& triangle_v     = triangle_vv[plane];
      auto& edge_v         = edge_vv[plane];
      auto& object_v       = object_vv[plane];

      object_v.resize(line_contour_v.size());

      for(size_t cid=0; cid<line_contour_v.size(); ++cid) {

	auto& triangle = triangle_v[cid];
	auto& edge = edge_v[cid];

	auto new_ctor = triangle.Contour();
	auto& object = object_v[cid];

	object._polygon_v.emplace_back(new_ctor,triangle.Apex());

	float npar_pixels = (float)larocv::CountNonZero(larocv::MaskImage(cimg,new_ctor,0,false));

	for(const auto ctor : ctor_overlap_vvv[plane][cid]) {

	  for(const auto& pt : *ctor)
	    new_ctor.emplace_back(pt);
	  
	  npar_pixels += (float)larocv::CountNonZero(larocv::MaskImage(cimg,*ctor,0,false));
	  object._polygon_v.emplace_back(*ctor,triangle.Apex());
	}

	triangle = Triangle(new_ctor,triangle.Apex());

	float nline_pixels = 0;
	auto line_ctor = MaximizeLine(cimg,triangle,nline_pixels,edge);

	float nratio = 0;

	if (npar_pixels > 0)
	  nratio = nline_pixels / npar_pixels;

	object._triangle  = triangle;
	object._line      = line_ctor;
	object._line_frac = nratio;
	object._edge      = edge;
	object._plane     = plane;
     
      }
    }

    _Match.ClearEvent();
    _Match.ClearMatch();

    LLCV_DEBUG() << "Match lines" << std::endl;
    for(size_t plane=0; plane<3; ++plane) {
      LLCV_DEBUG() << "@plane=" << plane << std::endl;
      for(const auto& object : object_vv[plane]) {
	_Match.Register(object,plane);
      }
      LLCV_DEBUG() << "num register=" << object_vv[plane].size() << std::endl;
    }
    
    std::vector<float> score_v;
    auto match_vv = _Match.MatchObjects(score_v);

    LLCV_DEBUG() << "& recieved " << match_vv.size() << " matched particles" << std::endl;
    
    std::vector<Object2DCollection> obj_col_v;
    obj_col_v.resize(match_vv.size());

    for(size_t mid=0; mid< match_vv.size(); ++mid) {
      auto match_v = match_vv[mid];
      auto score = score_v[mid];

      auto& obj_col = obj_col_v[mid];
      
      obj_col.SetStart(Data().Vertex()->X(),
		       Data().Vertex()->Y(),
		       Data().Vertex()->Z());
      
      obj_col.SetScore(score);

      // Fill the match
      for (auto match : match_v) {
	auto plane = match.first;
	auto id    = match.second;
	LLCV_DEBUG() << "@plane=" << plane << " id=" << id << std::endl;
	obj_col.emplace_back(object_vv[plane][id]);
      }
     
      ReconstructAngle(aimg_v,obj_col);
      ReconstructLength(img_v,aimg_v,obj_col);
      
      LLCV_DEBUG() << "theta=" << obj_col.Theta() << " phi=" << obj_col.Phi() << std::endl;
      LLCV_DEBUG() << "(" << obj_col.dX() << "," << obj_col.dY() << "," << obj_col.dZ() << ")" << std::endl;
      LLCV_DEBUG() << "length=" << obj_col.Length() << std::endl;
      LLCV_DEBUG() << "score=" << score << std::endl;
      LLCV_DEBUG() << "..." << std::endl;
    }

    
    //
    // Detect brem function
    //
    LLCV_DEBUG() << "Detecting brem" << std::endl;
    // mask out all polygons from all particles, find contours in cimg
    std::array<larocv::GEO2D_ContourArray_t,3> cimg_ctor_vv;
    for(size_t plane=0; plane<3; ++plane) {
      auto cimg_mask = cimg_v[plane].clone();
      auto& cimg_ctor_v = cimg_ctor_vv[plane];
      
      for(size_t oid=0; oid<obj_col_v.size(); ++oid) {
	auto& obj_col = obj_col_v[oid];
	if (!obj_col.HasObject(plane)) continue;
	const auto& obj2d = obj_col.PlaneObject(plane);
	cimg_mask = larocv::MaskImage(cimg_mask,geo2d::Circle<float>(obj2d.triangle().Apex(),5),-1,true);
	for(size_t polyid=0; polyid<obj2d.Polygons().size(); ++polyid) {
	  const auto& polygon = obj2d.Polygons()[polyid];
	  cimg_mask = larocv::MaskImage(cimg_mask,polygon.Contour(),-1,true);
	} // end polygon
      } // end object collection


      cimg_ctor_v = larocv::FindContours(cimg_mask);
    } // end plane
    
    
    for(size_t oid=0; oid<obj_col_v.size(); ++oid) {
      auto& obj_col = obj_col_v[oid];

      for(size_t plane=0; plane<3; ++plane) {
	if (!obj_col.HasObject(plane)) continue;
	auto& obj2d = obj_col.PlaneObjectRW(plane);
	
	const auto& cimg_ctor_v = cimg_ctor_vv[plane];

	// orient a triangle in direction of line
	auto edge = obj2d.Edge();

	obj2d._brem_triangle = obj2d.triangle().RotateToPoint(edge,2.0);
	obj2d._brem_triangle.SetContour(obj2d.triangle().Contour());
	
	// look for brem
	std::vector<size_t> brem_v;
	std::vector<size_t> inside_v;
	auto nbrem = DetectBrem(obj2d._brem_triangle,cimg_ctor_v,brem_v,inside_v);
	
	obj2d._n_brem = nbrem;

	// store all the contours inside the expanded triangle
	obj2d._expand_polygon_v = obj2d._polygon_v;

	for(auto iid : inside_v)
	  obj2d._expand_polygon_v.emplace_back(cimg_ctor_v[iid],obj2d.Start());
	
	if (!brem_v.empty())
	  obj2d._brem_index = obj2d._polygon_v.size();
	else
	  obj2d._brem_index = -1;

	// store the brem only into polygon
	for(auto bid : brem_v)
	  obj2d._polygon_v.emplace_back(cimg_ctor_v[bid],obj2d.Start());

      }
    }
    LLCV_DEBUG() << "...done" << std::endl;


    //
    // Detect branches for obj2d polygons
    //
    LLCV_DEBUG() << "Detecting branches..." << std::endl;
    for(auto& obj_col : obj_col_v) {
      for(size_t plane=0; plane<3; ++plane) {
	if (!obj_col.HasObject(plane)) continue;
	const auto& cimg = cimg_v[plane];
	auto& obj2d = obj_col.PlaneObjectRW(plane);
	for(auto& polygon : obj2d._polygon_v) 
	  polygon.DetectBranching(cimg,4,2,5,5);
      }
    }
    LLCV_DEBUG() << "...done" << std::endl;
    
    //
    // Write out
    //
    ResetEvent();

    // number of matched particles
    _n_par = (int)obj_col_v.size();

    // distance of vertex contour to edge
    for(size_t plane=0; plane<3; ++plane) {
      const auto& vertex_ctor = vertex_ctor_v[plane];
      auto& edge_dist = _edge_dist_v[plane];
      edge_dist = MinimizeToEdge(vertex_ctor);
    }

    // cosmic stuff
    for(size_t plane=0; plane<3; ++plane) {
      const auto& vertex_pt = vertex_pt_v[plane];
      const auto& _CosmicTag = _CosmicTag_v[plane];

      auto& edge_n_cosmic        = _edge_n_cosmic_v[plane];
      auto& edge_cosmic_vtx_dist = _edge_cosmic_vtx_dist_v[plane];

      edge_n_cosmic = (int)_CosmicTag.CosmicContours().size();

      auto near_id = _CosmicTag.NearestCosmicToPoint(vertex_pt,edge_cosmic_vtx_dist);
    }

    // object collection stuff
    for(size_t oid=0; oid<obj_col_v.size(); ++oid) {
      const auto& obj_col = obj_col_v[oid];
      
      LLCV_DEBUG() << "@oid=" << oid << std::endl;

      SetParticle(oid);
      
      *_par_theta    = obj_col.Theta();
      *_par_phi      = obj_col.Phi();
      *_par_length   = obj_col.Length();
      *_par_score    = obj_col.Score();
      *_par_dx       = obj_col.dX();
      *_par_dy       = obj_col.dY();
      *_par_dz       = obj_col.dZ();
      *_par_nplanes  = (int)obj_col.size();
      *_par_planes_v = obj_col.Planes();

      _white_img.setTo(cv::Scalar(1));
      *_par_xdead_v  = obj_col.XDead(dimg_v,_white_img);

      for(size_t plane=0; plane<3; ++plane) {
	LLCV_DEBUG() << "@plane=" << plane << std::endl;
	if (!obj_col.HasObject(plane)) continue;
	LLCV_DEBUG() << "...exists" << std::endl;

	const auto& obj2d = obj_col.PlaneObject(plane);

	size_t npolygons = obj2d.NPolygons();

	SetParticlePlane(oid,plane);
	
	*_par_n_polygons  = obj2d.NPolygons();
	*_par_linelength  = obj2d.LineLength();
	*_par_linefrac    = obj2d.LineFrac();   
	*_par_linedx      = obj2d.LinedX();
	*_par_linedy      = obj2d.LinedY();

	*_par_triangle_height         = obj2d.triangle().Height();
	*_par_triangle_emptyarearatio = obj2d.triangle().EmptyAreaRatio();
	*_par_triangle_emptyarea      = obj2d.triangle().EmptyArea();
	*_par_triangle_baselength     = obj2d.triangle().BaseLength();
	*_par_triangle_area           = obj2d.triangle().Area();
	*_par_triangle_brem           = obj2d.NBrem();
	*_par_triangle_coverage       = obj2d.brem_triangle().Coverage(aimg_v[plane]);
	*_par_expand_charge           = obj2d.Charge(aimg_v[plane]);
	*_par_length3d                = obj2d.Length();
	*_par_brem_idx                = obj2d.BremIndex();
	*_par_showerfrac              = obj2d.Fraction(*shr_v[plane],aimg_v[plane]);
	
	(*_par_cosmic_dist_v)[plane] = NearestPolygonToCosmic(obj2d.Polygons(),plane);

	ResizePlanePolygon(npolygons);

	for(size_t polyid=0; polyid<npolygons; ++polyid) {
	  const auto& polygon = obj2d.Polygons()[polyid];
	  (*_par_numberdefects_v)[polyid]     = polygon.NumberDefects(5);
	  (*_par_numberdefects_ns_v)[polyid]  = polygon.NumberDefectsNoStart(5);
	  (*_par_largestdefect_v)[polyid]     = polygon.LargestDefect();
	  (*_par_smallestdefect_v)[polyid]    = polygon.SmallestDefect();
	  (*_par_largestdefect_ns_v)[polyid]  = polygon.LargestDefectNoStart();
	  (*_par_smallestdefect_ns_v)[polyid] = polygon.SmallestDefectNoStart();
	  (*_par_emptyarearatio_v)[polyid]    = polygon.EmptyAreaRatio();
	  (*_par_emptyarea_v)[polyid]         = polygon.EmptyArea();
	  (*_par_pocketarea_v)[polyid]        = polygon.PocketArea();
	  (*_par_pocketarea_ns_v)[polyid]     = polygon.PocketAreaNoStart();
	  (*_par_polyarea_v)[polyid]          = polygon.Area();
	  (*_par_polyperimeter_v)[polyid]     = polygon.Perimeter();
	  (*_par_polycharge_v)[polyid]        = polygon.Charge(aimg_v[plane]);
	  (*_par_polyedges_v)[polyid]         = (int)polygon.Edges().size();
	  (*_par_polybranches_v)[polyid]      = (int)polygon.Branches().size();
	  (*_par_showerfrac_v)[polyid]        = (int)polygon.Fraction(*shr_v[plane],aimg_v[plane]);
	}


      }

    }


    if (_ismc) {

      auto seg_mat_v = Image().Image<cv::Mat>(kImageTrack,_cropx,_cropy);
      
      std::array<cv::Mat,3> electron_mat_v;
      std::array<cv::Mat,3> muon_mat_v;
      std::array<cv::Mat,3> proton_mat_v;

      for(size_t plane=0; plane<3; ++plane) {
	const auto& seg_mat = *(seg_mat_v[plane]);

	auto& electron_mat = electron_mat_v[plane];
	auto& muon_mat     = muon_mat_v[plane];
	auto& proton_mat   = proton_mat_v[plane];
	
	electron_mat = larocv::BlankImage(seg_mat,0);
	muon_mat     = larocv::BlankImage(seg_mat,0);
	proton_mat   = larocv::BlankImage(seg_mat,0);

	cv::inRange(seg_mat,3,3,electron_mat);
	cv::inRange(seg_mat,6,6,muon_mat);
	cv::inRange(seg_mat,9,9,proton_mat);
      }

      for(size_t oid=0; oid<obj_col_v.size(); ++oid) {
	const auto& obj_col = obj_col_v[oid];
	for(size_t plane=0; plane<3; ++plane) {
	  if (!obj_col.HasObject(plane)) continue;
	  
	  const auto& obj2d = obj_col.PlaneObject(plane);
	  SetSegmentPlane(oid,plane);	  
	  
	  const auto& adc_mat      = timg_v[plane];

	  const auto& electron_mat = electron_mat_v[plane];
	  const auto& muon_mat     = muon_mat_v[plane];
	  const auto& proton_mat   = proton_mat_v[plane];

	  // get the area fraction
	  float poly_area     = 0.0;
	  float electron_area = 0.0;
	  float muon_area     = 0.0;
	  float proton_area   = 0.0;

	  for(const auto& poly : obj2d.Polygons()) {
	    poly_area     += larocv::CountNonZero(larocv::MaskImage(adc_mat,poly.Contour(),-1,false));
	    electron_area += larocv::CountNonZero(larocv::MaskImage(electron_mat,poly.Contour(),-1,false));
	    muon_area     += larocv::CountNonZero(larocv::MaskImage(muon_mat,poly.Contour(),-1,false));
	    proton_area   += larocv::CountNonZero(larocv::MaskImage(proton_mat,poly.Contour(),-1,false));
	  }

	  *_par_electron_frac = electron_area / poly_area;
	  *_par_muon_frac     = muon_area / poly_area;
	  *_par_proton_frac   = proton_area / poly_area;

	} // end plane
	
      } // end obj_col     
      
    } // end ismc

    _outtree->Fill();
    
    
    //
    // Debug print out
    //
    if (_debug) {
      for(size_t plane=0; plane<3; ++plane) {
	
	auto write_img = timg_v[plane].clone();

	auto& _CosmicTag = _CosmicTag_v[plane];

	for(const auto& ctor : _CosmicTag.CosmicContours())
	  write_img = larocv::MaskImage(write_img,ctor,-1,true);

	auto mat3d = As8UC3(write_img);

	for(size_t oid=0; oid<obj_col_v.size(); ++oid) {
	  const auto& obj_col = obj_col_v[oid];
	  if (!obj_col.HasObject(plane)) continue;
	  const auto& obj2d = obj_col.PlaneObject(plane);
	  
	  for(const auto& polygon : obj2d.Polygons()) {
	    for(const auto& branch : polygon.Branches()) 
	      mat3d.at<cv::Vec3b>(branch.y,branch.x) = {255,51,255};
	    for(const auto& edge : polygon.Edges()) 
	      mat3d.at<cv::Vec3b>(edge.y,edge.x) = {0,128,255};
	  }

	  cv::drawContours(mat3d,larocv::GEO2D_ContourArray_t(1,obj2d.Line()),-1,cv::Scalar(255,255,0));
	  cv::drawContours(mat3d,larocv::GEO2D_ContourArray_t(1,obj2d.triangle().AsContour()),-1,cv::Scalar(0,255,255));
	  
	  const auto & tri_brem = obj2d.brem_triangle();
	  cv::drawContours(mat3d,larocv::GEO2D_ContourArray_t(1,tri_brem.AsContour()),-1,cv::Scalar(255,176,102));
	  
	  for(const auto& poly : obj2d.ExpandedPolygons()) 
	    cv::drawContours(mat3d,larocv::GEO2D_ContourArray_t(1,poly.Hull()),-1,cv::Scalar(0,0,255));

	  // for(const auto& poly : obj2d.Polygons()) 
	  //   cv::drawContours(mat3d,larocv::GEO2D_ContourArray_t(1,poly.Hull()),-1,cv::Scalar(0,255,0));

	}
      
	// _CosmicTag.DrawLines(mat3d);
	_CosmicTag.DrawContours(mat3d);
	
	std::stringstream ss;

	ss.str("");
	ss << "cpng/plane_img_" << Run() << "_" << SubRun() << "_" << Event() << "_" << VertexID() << "_" << plane << ".png";
	cv::imwrite(ss.str(),mat3d);
      }
    }
    
    LLCV_DEBUG() << "end" << std::endl;

    return 0.0;
  }

  size_t SelNueID::FindClosestContour(const larocv::GEO2D_ContourArray_t& ctor_v,
				      const geo2d::Vector<float>& pt,
				      float& distance) {

    size_t res;
    
    // get the contour closest to the vertex
    float dist = kINVALID_FLOAT;
    size_t close_id = kINVALID_SIZE;
    for(size_t cid=0; cid<ctor_v.size(); ++cid) {
      const auto& ctor = ctor_v[cid];
      auto dist_tmp = larocv::Pt2PtDistance(pt,ctor);
      if (dist_tmp < dist) {
	dist = dist_tmp;
	close_id = cid;
      }
    }
    
    distance = dist;
    res = close_id;
    return res;
  }
  
  larocv::GEO2D_Contour_t SelNueID::MaximizeLine(const cv::Mat& timg3d_mask,
						 const Triangle& triangle,
						 float& nline_pixels,
						 geo2d::Vector<float>& edge) {
    
    larocv::GEO2D_Contour_t res;

    auto timg3d_par   = larocv::MaskImage(timg3d_mask,triangle.Contour(),-1,false);
    
    nline_pixels = -1*larocv::kINVALID_FLOAT;

    const geo2d::Vector<float>* base_left  = nullptr;
    const geo2d::Vector<float>* base_right = nullptr;

    const geo2d::Vector<float>* base_up   = nullptr;
    const geo2d::Vector<float>* base_down = nullptr;

    bool isinf = false;
    
    if (triangle.Base1().x < triangle.Base2().x) {
      base_left  = &triangle.Base1();
      base_right = &triangle.Base2();
    }
    else if(triangle.Base1().x > triangle.Base2().x) {
      base_left  = &triangle.Base2();
      base_right = &triangle.Base1();
    }
    // it's infinity
    else {
      isinf = true;
      if (triangle.Base1().y < triangle.Base2().y) {
	base_down = &triangle.Base1();
	base_up   = &triangle.Base2();
      }
      else {
	base_down = &triangle.Base2();
	base_up   = &triangle.Base1();
      }
    }

    geo2d::LineSegment<float> base_seg;
    
    float lo;
    float hi; 

    if (!isinf)  {
      base_seg = geo2d::LineSegment<float>(*base_left,*base_right);
      lo = base_left->x;
      hi = base_right->x;
    }
    else {
      base_seg = geo2d::LineSegment<float>(*base_down,*base_up);
      lo = base_down->y;
      hi = base_up->y;
    }

    geo2d::Vector<float> pt;
    
    float step = 0.1;
    for(float rid=lo; rid<hi; rid+=step) {
      
      _white_img.setTo(cv::Scalar(0));
      
      if (!isinf) {
	pt.x = rid;
	pt.y = -1;
	try {
	  pt.y = base_seg.y(pt.x);
	}
	catch (...) {
	  continue;
	}
      } 
      
      else {
	pt.x = base_down->x;
	pt.y = rid;
      }

      // draw a line
      cv::line(_white_img,triangle.Apex(),pt,cv::Scalar(255),3);
      
      // find the contour
      auto line_ctor_v = larocv::FindContours(_white_img);
      if (line_ctor_v.empty()) continue;
      const auto& line_ctor = line_ctor_v.front();
      
      if (line_ctor.empty()) continue;
      
      auto timg3d_par_line = larocv::MaskImage(timg3d_par,line_ctor,-1,false);
      float nline_pixels_tmp = (float)larocv::CountNonZero(timg3d_par_line);
      
      if (nline_pixels_tmp > nline_pixels) {
	nline_pixels = nline_pixels_tmp;
	edge = pt;
	res = line_ctor;
      }
      
    }

    return res;
  }
  

  //
  // adapted from https://goo.gl/Q7MBpE
  //
  void SelNueID::ReconstructAngle(const std::array<cv::Mat,3>& img_v,
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
	auto nz_pt_v = larocv::FindNonZero(larocv::MaskImage(img_v[pl],poly.Contour(),-1,false));
	
	for (const auto& nz_pt  : nz_pt_v){
	  float charge = (float) (img_v[pl].at<uchar>(nz_pt.y,nz_pt.x));
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

  void SelNueID::ReconstructLength(const std::vector<larcv::Image2D*>& img_v,
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
	for (const auto& nz_pt  : nz_pt_v)
	  qsum += (float) (aimg_v[pl].at<uchar>(nz_pt.y,nz_pt.x));
      }

      for(const auto& poly : obj._polygon_v) {
	auto nz_pt_v = larocv::FindNonZero(larocv::MaskImage(aimg_v[pl],poly.Contour(),-1,false));
     
	for(size_t nid=0; nid < nz_pt_v.size(); ++nid) {
	  auto nz_pt = nz_pt_v[nid];

	  float charge = (float) (aimg_v[pl].at<uchar>(nz_pt.y,nz_pt.x));

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
	if (qsum>_qfraction) break;
      }
      
      auto f = plane_f_v.at(pl);
      
      double length = d2D / f;

      LLCV_DEBUG() << "@pl=" << pl << " f=" << f << " length=" << length << std::endl;
      length_v[pl] = length;
    }
    
    // auto length = *(std::max_element(std::begin(length_v),std::end(length_v)));

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

  void SelNueID::ResetEvent() {
    
    _n_par     = -1.0*larocv::kINVALID_INT;
    _edge_dist_v.clear();
    _edge_dist_v.resize(3,-1);
    _edge_n_cosmic_v.clear();
    _edge_n_cosmic_v.resize(3,-1);
    _edge_cosmic_vtx_dist_v.clear();
    _edge_cosmic_vtx_dist_v.resize(3,-1);

    //
    // 3D information
    //
    _par1_theta    = -1.0*larocv::kINVALID_FLOAT;
    _par1_phi      = -1.0*larocv::kINVALID_FLOAT;
    _par1_length   = -1.0*larocv::kINVALID_FLOAT;
    _par1_score    = -1.0*larocv::kINVALID_FLOAT;
    _par1_dx       = -1.0*larocv::kINVALID_FLOAT;
    _par1_dy       = -1.0*larocv::kINVALID_FLOAT;
    _par1_dz       = -1.0*larocv::kINVALID_FLOAT;
    _par1_nplanes  = -1*larocv::kINVALID_INT;
    _par1_planes_v.clear();
    _par1_planes_v.resize(3,-1);
    _par1_xdead_v.clear();
    _par1_xdead_v.resize(3,-1);
    _par1_cosmic_dist_v.clear();
    _par1_cosmic_dist_v.resize(3,-1);    

    _par2_theta   = -1.0*larocv::kINVALID_FLOAT;
    _par2_phi     = -1.0*larocv::kINVALID_FLOAT;
    _par2_length  = -1.0*larocv::kINVALID_FLOAT;
    _par2_score   = -1.0*larocv::kINVALID_FLOAT;
    _par2_dx      = -1.0*larocv::kINVALID_FLOAT;
    _par2_dy      = -1.0*larocv::kINVALID_FLOAT;
    _par2_dz      = -1.0*larocv::kINVALID_FLOAT;
    _par2_nplanes = -1*larocv::kINVALID_INT;
    _par2_planes_v.clear();
    _par2_planes_v.resize(3,-1);
    _par2_xdead_v.clear();
    _par2_xdead_v.resize(3,-1);
    _par2_cosmic_dist_v.clear();
    _par2_cosmic_dist_v.resize(3,-1);

    _par_theta         = nullptr;
    _par_phi           = nullptr;
    _par_length        = nullptr;
    _par_score         = nullptr;
    _par_dx            = nullptr;
    _par_dy            = nullptr;
    _par_dz            = nullptr;
    _par_nplanes       = nullptr;
    _par_planes_v      = nullptr;
    _par_xdead_v       = nullptr;
    _par_cosmic_dist_v = nullptr;

    //
    // 2D information
    //

    _par1_n_polygons_U = -1*larocv::kINVALID_INT;
    _par1_n_polygons_V = -1*larocv::kINVALID_INT;
    _par1_n_polygons_Y = -1*larocv::kINVALID_INT;
    _par2_n_polygons_U = -1*larocv::kINVALID_INT;
    _par2_n_polygons_V = -1*larocv::kINVALID_INT;
    _par2_n_polygons_Y = -1*larocv::kINVALID_INT;
    _par_n_polygons = nullptr;

    _par1_linelength_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_linelength_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_linelength_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_linelength_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_linelength_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_linelength_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_linelength = nullptr;

    _par1_linefrac_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_linefrac_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_linefrac_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_linefrac_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_linefrac_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_linefrac_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_linefrac = nullptr;

    _par1_linedx_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_linedx_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_linedx_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_linedx_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_linedx_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_linedx_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_linedx = nullptr;

    _par1_linedy_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_linedy_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_linedy_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_linedy_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_linedy_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_linedy_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_linedy = nullptr;

    _par1_triangle_height_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_triangle_height_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_triangle_height_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_height_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_height_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_height_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_triangle_height = nullptr;

    _par1_triangle_emptyarearatio_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_triangle_emptyarearatio_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_triangle_emptyarearatio_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_emptyarearatio_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_emptyarearatio_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_emptyarearatio_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_triangle_emptyarearatio = nullptr;

    _par1_triangle_emptyarea_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_triangle_emptyarea_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_triangle_emptyarea_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_emptyarea_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_emptyarea_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_emptyarea_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_triangle_emptyarea = nullptr;
    
    _par1_triangle_baselength_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_triangle_baselength_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_triangle_baselength_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_baselength_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_baselength_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_baselength_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_triangle_baselength = nullptr;

    _par1_triangle_area_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_triangle_area_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_triangle_area_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_area_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_area_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_area_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_triangle_area = nullptr;

    _par1_triangle_brem_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_triangle_brem_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_triangle_brem_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_brem_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_brem_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_brem_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_triangle_brem = nullptr;

    _par1_triangle_coverage_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_triangle_coverage_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_triangle_coverage_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_coverage_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_coverage_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_triangle_coverage_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_triangle_coverage = nullptr;

    _par1_expand_charge_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_expand_charge_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_expand_charge_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_expand_charge_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_expand_charge_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_expand_charge_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_expand_charge = nullptr;
    
    _par1_brem_idx_U = -1.0*larocv::kINVALID_INT;
    _par1_brem_idx_V = -1.0*larocv::kINVALID_INT;
    _par1_brem_idx_Y = -1.0*larocv::kINVALID_INT;
    _par2_brem_idx_U = -1.0*larocv::kINVALID_INT;
    _par2_brem_idx_V = -1.0*larocv::kINVALID_INT;
    _par2_brem_idx_Y = -1.0*larocv::kINVALID_INT;
    _par_brem_idx = nullptr;
    
    _par1_length3d_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_length3d_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_length3d_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_length3d_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_length3d_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_length3d_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_length3d = nullptr;

    _par1_showerfrac_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_showerfrac_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_showerfrac_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_showerfrac_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_showerfrac_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_showerfrac_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_showerfrac = nullptr;
    
    _par1_numberdefects_U_v.clear();
    _par1_numberdefects_V_v.clear();
    _par1_numberdefects_Y_v.clear();
    _par2_numberdefects_U_v.clear();
    _par2_numberdefects_V_v.clear();
    _par2_numberdefects_Y_v.clear();
    _par_numberdefects_v = nullptr;

    _par1_numberdefects_ns_U_v.clear();
    _par1_numberdefects_ns_V_v.clear();
    _par1_numberdefects_ns_Y_v.clear();
    _par2_numberdefects_ns_U_v.clear();
    _par2_numberdefects_ns_V_v.clear();
    _par2_numberdefects_ns_Y_v.clear();
    _par_numberdefects_ns_v = nullptr;
    
    _par1_largestdefect_U_v.clear();
    _par1_largestdefect_V_v.clear();
    _par1_largestdefect_Y_v.clear();
    _par2_largestdefect_U_v.clear();
    _par2_largestdefect_V_v.clear();
    _par2_largestdefect_Y_v.clear();
    _par_largestdefect_v = nullptr;

    _par1_smallestdefect_U_v.clear();
    _par1_smallestdefect_V_v.clear();
    _par1_smallestdefect_Y_v.clear();
    _par2_smallestdefect_U_v.clear();
    _par2_smallestdefect_V_v.clear();
    _par2_smallestdefect_Y_v.clear();
    _par_smallestdefect_v = nullptr;

    _par1_largestdefect_ns_U_v.clear();
    _par1_largestdefect_ns_V_v.clear();
    _par1_largestdefect_ns_Y_v.clear();
    _par2_largestdefect_ns_U_v.clear();
    _par2_largestdefect_ns_V_v.clear();
    _par2_largestdefect_ns_Y_v.clear();
    _par_largestdefect_ns_v = nullptr;

    _par1_smallestdefect_ns_U_v.clear();
    _par1_smallestdefect_ns_V_v.clear();
    _par1_smallestdefect_ns_Y_v.clear();
    _par2_smallestdefect_ns_U_v.clear();
    _par2_smallestdefect_ns_V_v.clear();
    _par2_smallestdefect_ns_Y_v.clear();
    _par_smallestdefect_ns_v = nullptr;

    _par1_emptyarearatio_U_v.clear();
    _par1_emptyarearatio_V_v.clear();
    _par1_emptyarearatio_Y_v.clear();
    _par2_emptyarearatio_U_v.clear();
    _par2_emptyarearatio_V_v.clear();
    _par2_emptyarearatio_Y_v.clear();
    _par_emptyarearatio_v = nullptr;

    _par1_emptyarea_U_v.clear();
    _par1_emptyarea_V_v.clear();
    _par1_emptyarea_Y_v.clear();
    _par2_emptyarea_U_v.clear();
    _par2_emptyarea_V_v.clear();
    _par2_emptyarea_Y_v.clear();
    _par_emptyarea_v = nullptr;

    _par1_pocketarea_U_v.clear();
    _par1_pocketarea_V_v.clear();
    _par1_pocketarea_Y_v.clear();
    _par2_pocketarea_U_v.clear();
    _par2_pocketarea_V_v.clear();
    _par2_pocketarea_Y_v.clear();
    _par_pocketarea_v = nullptr;

    _par1_pocketarea_ns_U_v.clear();
    _par1_pocketarea_ns_V_v.clear();
    _par1_pocketarea_ns_Y_v.clear();
    _par2_pocketarea_ns_U_v.clear();
    _par2_pocketarea_ns_V_v.clear();
    _par2_pocketarea_ns_Y_v.clear();
    _par_pocketarea_ns_v = nullptr;

    _par1_polyarea_U_v.clear();
    _par1_polyarea_V_v.clear();
    _par1_polyarea_Y_v.clear();
    _par2_polyarea_U_v.clear();
    _par2_polyarea_V_v.clear();
    _par2_polyarea_Y_v.clear();
    _par_polyarea_v = nullptr;

    _par1_polyperimeter_U_v.clear();
    _par1_polyperimeter_V_v.clear();
    _par1_polyperimeter_Y_v.clear();
    _par2_polyperimeter_U_v.clear();
    _par2_polyperimeter_V_v.clear();
    _par2_polyperimeter_Y_v.clear();
    _par_polyperimeter_v = nullptr;

    _par1_polycharge_U_v.clear();
    _par1_polycharge_V_v.clear();
    _par1_polycharge_Y_v.clear();
    _par2_polycharge_U_v.clear();
    _par2_polycharge_V_v.clear();
    _par2_polycharge_Y_v.clear();
    _par_polycharge_v = nullptr;

    _par1_polyedges_U_v.clear();
    _par1_polyedges_V_v.clear();
    _par1_polyedges_Y_v.clear();
    _par2_polyedges_U_v.clear();
    _par2_polyedges_V_v.clear();
    _par2_polyedges_Y_v.clear();
    _par_polyedges_v = nullptr;

    _par1_polybranches_U_v.clear();
    _par1_polybranches_V_v.clear();
    _par1_polybranches_Y_v.clear();
    _par2_polybranches_U_v.clear();
    _par2_polybranches_V_v.clear();
    _par2_polybranches_Y_v.clear();
    _par_polybranches_v = nullptr;

    _par1_showerfrac_U_v.clear();
    _par1_showerfrac_V_v.clear();
    _par1_showerfrac_Y_v.clear();
    _par2_showerfrac_U_v.clear();
    _par2_showerfrac_V_v.clear();
    _par2_showerfrac_Y_v.clear();
    _par_showerfrac_v = nullptr;

    _par1_electron_frac_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_electron_frac_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_electron_frac_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_electron_frac_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_electron_frac_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_electron_frac_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_electron_frac = nullptr;

    _par1_muon_frac_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_muon_frac_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_muon_frac_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_muon_frac_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_muon_frac_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_muon_frac_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_muon_frac = nullptr;

    _par1_proton_frac_U = -1.0*larocv::kINVALID_FLOAT;
    _par1_proton_frac_V = -1.0*larocv::kINVALID_FLOAT;
    _par1_proton_frac_Y = -1.0*larocv::kINVALID_FLOAT;
    _par2_proton_frac_U = -1.0*larocv::kINVALID_FLOAT;
    _par2_proton_frac_V = -1.0*larocv::kINVALID_FLOAT;
    _par2_proton_frac_Y = -1.0*larocv::kINVALID_FLOAT;
    _par_proton_frac = nullptr;

    return;
  }

  void SelNueID::ResizePlanePolygon(size_t sz) {

    if(!_par_numberdefects_v) {
      LLCV_CRITICAL() << "ptr to _par_numberdefects_v is null" << std::endl;
      throw llcv_err("die");
    }
    if(!_par_numberdefects_ns_v) {
      LLCV_CRITICAL() << "ptr to _par_numberdefects_ns_v is null" << std::endl;
      throw llcv_err("die");
    }
    if(!_par_largestdefect_v) {
      LLCV_CRITICAL() << "ptr to _par_largestdefect_v is null" << std::endl;
      throw llcv_err("die");
    }
    if(!_par_smallestdefect_v) {
      LLCV_CRITICAL() << "ptr to _par_smallestdefect_v is null" << std::endl;
      throw llcv_err("die");
    }
    if(!_par_largestdefect_ns_v) {
      LLCV_CRITICAL() << "ptr to _par_largestdefect_ns_v is null" << std::endl;
      throw llcv_err("die");
    }
    if(!_par_smallestdefect_ns_v) {
      LLCV_CRITICAL() << "ptr to _par_smallestdefect_ns_v is null" << std::endl;
      throw llcv_err("die");
    }
    if(!_par_emptyarearatio_v) {
      LLCV_CRITICAL() << "ptr to _par_emptyarearatio_v is null" << std::endl;
      throw llcv_err("die");
    }
    if(!_par_emptyarea_v) {
      LLCV_CRITICAL() << "ptr to _par_emptyarea_v is null" << std::endl;
      throw llcv_err("die");
    }
    if(!_par_pocketarea_v) {
      LLCV_CRITICAL() << "ptr to _par_pocketarea_v is null" << std::endl;
      throw llcv_err("die");
    }
    if(!_par_pocketarea_ns_v) {
      LLCV_CRITICAL() << "ptr to _par_pocketarea_ns_v is null" << std::endl;
      throw llcv_err("die");
    }
    if(!_par_polyarea_v) {
      LLCV_CRITICAL() << "ptr to _par_polyarea_v is null" << std::endl;
      throw llcv_err("die");
    }
    if(!_par_polyperimeter_v) {
      LLCV_CRITICAL() << "ptr to _par_polyperimeter_v is null" << std::endl;
      throw llcv_err("die");
    }
    if(!_par_polycharge_v) {
      LLCV_CRITICAL() << "ptr to _par_polycharge_v is null" << std::endl;
      throw llcv_err("die");
    }

    if(!_par_polyedges_v) {
      LLCV_CRITICAL() << "ptr to _par_polyedges_v is null" << std::endl;
      throw llcv_err("die");
    }

    if(!_par_polybranches_v) {
      LLCV_CRITICAL() << "ptr to _par_branches_v is null" << std::endl;
      throw llcv_err("die");
    }

    if(!_par_showerfrac_v) {
      LLCV_CRITICAL() << "ptr to _par_showerfrac_v is null" << std::endl;
      throw llcv_err("die");
    }

    
    _par_numberdefects_v->resize(sz,-1.0*larocv::kINVALID_INT);

    _par_numberdefects_ns_v->resize(sz,-1.0*larocv::kINVALID_INT);

    _par_largestdefect_v->resize(sz,-1.0*larocv::kINVALID_FLOAT);

    _par_smallestdefect_v->resize(sz,-1.0*larocv::kINVALID_FLOAT);

    _par_largestdefect_ns_v->resize(sz,-1.0*larocv::kINVALID_FLOAT);

    _par_smallestdefect_ns_v->resize(sz,-1.0*larocv::kINVALID_FLOAT);

    _par_emptyarearatio_v->resize(sz,-1.0*larocv::kINVALID_FLOAT);
    
    _par_emptyarea_v->resize(sz,-1.0*larocv::kINVALID_FLOAT);

    _par_pocketarea_v->resize(sz,-1.0*larocv::kINVALID_FLOAT);

    _par_pocketarea_ns_v->resize(sz,-1.0*larocv::kINVALID_FLOAT);

    _par_polyarea_v->resize(sz,-1.0*larocv::kINVALID_FLOAT);

    _par_polyperimeter_v->resize(sz,-1.0*larocv::kINVALID_FLOAT);

    _par_polycharge_v->resize(sz,-1.0*larocv::kINVALID_FLOAT);
    
    _par_polyedges_v->resize(sz,-1.0*larocv::kINVALID_INT);

    _par_polybranches_v->resize(sz,-1.0*larocv::kINVALID_INT);

    _par_showerfrac_v->resize(sz,-1.0*larocv::kINVALID_INT);

    return;
  }
  
  void SelNueID::SetParticle(size_t pid) {

    switch(pid) {
    case 0 : {
      _par_theta    = &_par1_theta;
      _par_phi      = &_par1_phi;
      _par_length   = &_par1_length;
      _par_score    = &_par1_score;
      _par_dx       = &_par1_dx;
      _par_dy       = &_par1_dy;
      _par_dz       = &_par1_dz;
      _par_nplanes  = &_par1_nplanes;
      _par_planes_v = &_par1_planes_v;
      _par_xdead_v  = &_par1_xdead_v;

      _par_cosmic_dist_v = &_par1_cosmic_dist_v;
      break;
    }
    case 1 : {
      _par_theta    = &_par2_theta;
      _par_phi      = &_par2_phi;
      _par_length   = &_par2_length;
      _par_score    = &_par2_score;
      _par_dx       = &_par2_dx;
      _par_dy       = &_par2_dy;
      _par_dz       = &_par2_dz;
      _par_nplanes  = &_par2_nplanes;
      _par_planes_v = &_par2_planes_v;
      _par_xdead_v  = &_par2_xdead_v;

      _par_cosmic_dist_v = &_par2_cosmic_dist_v;
      break;
    }
    default : { break; }
    }

    return;
  }

  void SelNueID::SetParticlePlane(size_t pid, size_t plane) {

    LLCV_DEBUG() << "@pid=" << pid << " @plane=" << plane << std::endl;

    switch(pid) {
    case 0 : {
      switch(plane) {
      case 0 : {
	_par_n_polygons              = &_par1_n_polygons_U;
	_par_linelength              = &_par1_linelength_U;
	_par_linefrac                = &_par1_linefrac_U;
	_par_linedx                  = &_par1_linedx_U;
	_par_linedy                  = &_par1_linedy_U;
	_par_triangle_height         = &_par1_triangle_height_U;
	_par_triangle_emptyarearatio = &_par1_triangle_emptyarearatio_U;
	_par_triangle_emptyarea      = &_par1_triangle_emptyarea_U;
	_par_triangle_baselength     = &_par1_triangle_baselength_U;
	_par_triangle_area           = &_par1_triangle_area_U;
	_par_triangle_brem           = &_par1_triangle_brem_U;	
	_par_triangle_coverage       = &_par1_triangle_coverage_U;
	_par_brem_idx                = &_par1_brem_idx_U;
	_par_expand_charge           = &_par1_expand_charge_U;
	_par_length3d                = &_par1_length3d_U;
	_par_showerfrac              = &_par1_showerfrac_U;

	_par_numberdefects_v     = &_par1_numberdefects_U_v;
	_par_numberdefects_ns_v  = &_par1_numberdefects_ns_U_v;
	_par_largestdefect_v     = &_par1_largestdefect_U_v;
	_par_smallestdefect_v    = &_par1_smallestdefect_U_v;
	_par_largestdefect_ns_v  = &_par1_largestdefect_ns_U_v;
	_par_smallestdefect_ns_v = &_par1_smallestdefect_ns_U_v;
	_par_emptyarearatio_v    = &_par1_emptyarearatio_U_v;
	_par_emptyarea_v         = &_par1_emptyarea_U_v;
	_par_pocketarea_v        = &_par1_pocketarea_U_v;
	_par_pocketarea_ns_v     = &_par1_pocketarea_ns_U_v;
	_par_polyarea_v          = &_par1_polyarea_U_v;
	_par_polyperimeter_v     = &_par1_polyperimeter_U_v;
	_par_polycharge_v        = &_par1_polycharge_U_v;
	_par_polyedges_v         = &_par1_polyedges_U_v;
	_par_polybranches_v      = &_par1_polybranches_U_v;
	_par_showerfrac_v        = &_par1_showerfrac_U_v;

	break;
      }
      case 1: {
	_par_n_polygons              = &_par1_n_polygons_V;
	_par_linelength              = &_par1_linelength_V;
	_par_linefrac                = &_par1_linefrac_V;
	_par_linedx                  = &_par1_linedx_V;
	_par_linedy                  = &_par1_linedy_V;
	_par_triangle_height         = &_par1_triangle_height_V;
	_par_triangle_emptyarearatio = &_par1_triangle_emptyarearatio_V;
	_par_triangle_emptyarea      = &_par1_triangle_emptyarea_V;
	_par_triangle_baselength     = &_par1_triangle_baselength_V;
	_par_triangle_area           = &_par1_triangle_area_V;
	_par_triangle_brem           = &_par1_triangle_brem_V;
	_par_triangle_coverage       = &_par1_triangle_coverage_V;
	_par_brem_idx                = &_par1_brem_idx_V;
	_par_expand_charge           = &_par1_expand_charge_V;
	_par_length3d                = &_par1_length3d_V;
	_par_showerfrac              = &_par1_showerfrac_V;

	_par_numberdefects_v     = &_par1_numberdefects_V_v;
	_par_numberdefects_ns_v  = &_par1_numberdefects_ns_V_v;
	_par_largestdefect_v     = &_par1_largestdefect_V_v;
	_par_smallestdefect_v    = &_par1_smallestdefect_V_v;
	_par_largestdefect_ns_v  = &_par1_largestdefect_ns_V_v;
	_par_smallestdefect_ns_v = &_par1_smallestdefect_ns_V_v;
	_par_emptyarearatio_v    = &_par1_emptyarearatio_V_v;
	_par_emptyarea_v         = &_par1_emptyarea_V_v;
	_par_pocketarea_v        = &_par1_pocketarea_V_v;
	_par_pocketarea_ns_v     = &_par1_pocketarea_ns_V_v;
	_par_polyarea_v          = &_par1_polyarea_V_v;
	_par_polyperimeter_v     = &_par1_polyperimeter_V_v;
	_par_polycharge_v        = &_par1_polycharge_V_v;
	_par_polyedges_v         = &_par1_polyedges_V_v;
	_par_polybranches_v      = &_par1_polybranches_V_v;
	_par_showerfrac_v        = &_par1_showerfrac_V_v;

	break;
      }
      case 2: {
	_par_n_polygons              = &_par1_n_polygons_Y;
	_par_linelength              = &_par1_linelength_Y;
	_par_linefrac                = &_par1_linefrac_Y;
	_par_linedx                  = &_par1_linedx_Y;
	_par_linedy                  = &_par1_linedy_Y;
	_par_triangle_height         = &_par1_triangle_height_Y;
	_par_triangle_emptyarearatio = &_par1_triangle_emptyarearatio_Y;
	_par_triangle_emptyarea      = &_par1_triangle_emptyarea_Y;
	_par_triangle_baselength     = &_par1_triangle_baselength_Y;
	_par_triangle_area           = &_par1_triangle_area_Y;
	_par_triangle_brem           = &_par1_triangle_brem_Y;
	_par_triangle_coverage       = &_par1_triangle_coverage_Y;
	_par_brem_idx                = &_par1_brem_idx_Y;
	_par_expand_charge           = &_par1_expand_charge_Y;
	_par_length3d                = &_par1_length3d_Y;
	_par_showerfrac              = &_par1_showerfrac_Y;

	_par_numberdefects_v     = &_par1_numberdefects_Y_v;
	_par_numberdefects_ns_v  = &_par1_numberdefects_ns_Y_v;
	_par_largestdefect_v     = &_par1_largestdefect_Y_v;
	_par_smallestdefect_v    = &_par1_smallestdefect_Y_v;
	_par_largestdefect_ns_v  = &_par1_largestdefect_ns_Y_v;
	_par_smallestdefect_ns_v = &_par1_smallestdefect_ns_Y_v;
	_par_emptyarearatio_v    = &_par1_emptyarearatio_Y_v;
	_par_emptyarea_v         = &_par1_emptyarea_Y_v;
	_par_pocketarea_v        = &_par1_pocketarea_Y_v;
	_par_pocketarea_ns_v     = &_par1_pocketarea_ns_Y_v;
	_par_polyarea_v          = &_par1_polyarea_Y_v;
	_par_polyperimeter_v     = &_par1_polyperimeter_Y_v;
	_par_polycharge_v        = &_par1_polycharge_Y_v;
	_par_polyedges_v         = &_par1_polyedges_Y_v;
	_par_polybranches_v      = &_par1_polybranches_Y_v;
	_par_showerfrac_v        = &_par1_showerfrac_Y_v;

	break;
      }
      default : { break; }
      } // end plane
      break;
    } // end particle 1

    case 1 : {
      switch(plane) {
      case 0 : {
	_par_n_polygons              = &_par2_n_polygons_U;
	_par_linelength              = &_par2_linelength_U;
	_par_linefrac                = &_par2_linefrac_U;
	_par_linedx                  = &_par2_linedx_U;
	_par_linedy                  = &_par2_linedy_U;
	_par_triangle_height         = &_par2_triangle_height_U;
	_par_triangle_emptyarearatio = &_par2_triangle_emptyarearatio_U;
	_par_triangle_emptyarea      = &_par2_triangle_emptyarea_U;
	_par_triangle_baselength     = &_par2_triangle_baselength_U;
	_par_triangle_area           = &_par2_triangle_area_U;
	_par_triangle_brem           = &_par2_triangle_brem_U;
	_par_triangle_coverage       = &_par2_triangle_coverage_U;
	_par_brem_idx                = &_par2_brem_idx_U;
	_par_expand_charge           = &_par2_expand_charge_U;
	_par_length3d                = &_par2_length3d_U;
	_par_showerfrac              = &_par2_showerfrac_U;

	_par_numberdefects_v     = &_par2_numberdefects_U_v;
	_par_numberdefects_ns_v  = &_par2_numberdefects_ns_U_v;
	_par_largestdefect_v     = &_par2_largestdefect_U_v;
	_par_smallestdefect_v    = &_par2_smallestdefect_U_v;
	_par_largestdefect_ns_v  = &_par2_largestdefect_ns_U_v;
	_par_smallestdefect_ns_v = &_par2_smallestdefect_ns_U_v;
	_par_emptyarearatio_v    = &_par2_emptyarearatio_U_v;
	_par_emptyarea_v         = &_par2_emptyarea_U_v;
	_par_pocketarea_v        = &_par2_pocketarea_U_v;
	_par_pocketarea_ns_v     = &_par2_pocketarea_ns_U_v;
	_par_polyarea_v          = &_par2_polyarea_U_v;
	_par_polyperimeter_v     = &_par2_polyperimeter_U_v;
	_par_polycharge_v        = &_par2_polycharge_U_v;
	_par_polyedges_v         = &_par2_polyedges_U_v;
	_par_polybranches_v      = &_par2_polybranches_U_v;
	_par_showerfrac_v        = &_par2_showerfrac_U_v;

	break;
      }
      case 1: {
	_par_n_polygons              = &_par2_n_polygons_V;
	_par_linelength              = &_par2_linelength_V;
	_par_linefrac                = &_par2_linefrac_V;
	_par_linedx                  = &_par2_linedx_V;
	_par_linedy                  = &_par2_linedy_V;
	_par_triangle_height         = &_par2_triangle_height_V;
	_par_triangle_emptyarearatio = &_par2_triangle_emptyarearatio_V;
	_par_triangle_emptyarea      = &_par2_triangle_emptyarea_V;
	_par_triangle_baselength     = &_par2_triangle_baselength_V;
	_par_triangle_area           = &_par2_triangle_area_V;
	_par_triangle_brem           = &_par2_triangle_brem_V;
	_par_triangle_coverage       = &_par2_triangle_coverage_V;
	_par_brem_idx                = &_par2_brem_idx_V;
	_par_expand_charge           = &_par2_expand_charge_V;
	_par_length3d                = &_par2_length3d_V;
	_par_showerfrac              = &_par2_showerfrac_V;

	_par_numberdefects_v     = &_par2_numberdefects_V_v;
	_par_numberdefects_ns_v  = &_par2_numberdefects_ns_V_v;
	_par_largestdefect_v     = &_par2_largestdefect_V_v;
	_par_smallestdefect_v    = &_par2_smallestdefect_V_v;
	_par_largestdefect_ns_v  = &_par2_largestdefect_ns_V_v;
	_par_smallestdefect_ns_v = &_par2_smallestdefect_ns_V_v;
	_par_emptyarearatio_v    = &_par2_emptyarearatio_V_v;
	_par_emptyarea_v         = &_par2_emptyarea_V_v;
	_par_pocketarea_v        = &_par2_pocketarea_V_v;
	_par_pocketarea_ns_v     = &_par2_pocketarea_ns_V_v;
	_par_polyarea_v          = &_par2_polyarea_V_v;
	_par_polyperimeter_v     = &_par2_polyperimeter_V_v;
	_par_polycharge_v        = &_par2_polycharge_V_v;
	_par_polyedges_v         = &_par2_polyedges_V_v;
	_par_polybranches_v      = &_par2_polybranches_V_v;
	_par_showerfrac_v        = &_par2_showerfrac_V_v;

	break;
      }
      case 2: {
	_par_n_polygons              = &_par2_n_polygons_Y;
	_par_linelength              = &_par2_linelength_Y;
	_par_linefrac                = &_par2_linefrac_Y;
	_par_linedx                  = &_par2_linedx_Y;
	_par_linedy                  = &_par2_linedy_Y;
	_par_triangle_height         = &_par2_triangle_height_Y;
	_par_triangle_emptyarearatio = &_par2_triangle_emptyarearatio_Y;
	_par_triangle_emptyarea      = &_par2_triangle_emptyarea_Y;
	_par_triangle_baselength     = &_par2_triangle_baselength_Y;
	_par_triangle_area           = &_par2_triangle_area_Y;
	_par_triangle_brem           = &_par2_triangle_brem_Y;
	_par_triangle_coverage       = &_par2_triangle_coverage_Y;
	_par_brem_idx                = &_par2_brem_idx_Y;
	_par_expand_charge           = &_par2_expand_charge_Y;
	_par_length3d                = &_par2_length3d_Y;
	_par_showerfrac              = &_par2_showerfrac_Y;

	_par_numberdefects_v     = &_par2_numberdefects_Y_v;
	_par_numberdefects_ns_v  = &_par2_numberdefects_ns_Y_v;
	_par_largestdefect_v     = &_par2_largestdefect_Y_v;
	_par_smallestdefect_v    = &_par2_smallestdefect_Y_v;
	_par_largestdefect_ns_v  = &_par2_largestdefect_ns_Y_v;
	_par_smallestdefect_ns_v = &_par2_smallestdefect_ns_Y_v;
	_par_emptyarearatio_v    = &_par2_emptyarearatio_Y_v;
	_par_emptyarea_v         = &_par2_emptyarea_Y_v;
	_par_pocketarea_v        = &_par2_pocketarea_Y_v;
	_par_pocketarea_ns_v     = &_par2_pocketarea_ns_Y_v;
	_par_polyarea_v          = &_par2_polyarea_Y_v;
	_par_polyperimeter_v     = &_par2_polyperimeter_Y_v;
	_par_polycharge_v        = &_par2_polycharge_Y_v;
	_par_polyedges_v         = &_par2_polyedges_Y_v;
	_par_polybranches_v      = &_par2_polybranches_Y_v;
	_par_showerfrac_v        = &_par2_showerfrac_Y_v;

	break;
      }
      default : { break; }
      } // end plane
      break;
    } // end particle 2
    default : { break; }
    } // end particle
    return;
  }

void SelNueID::SetSegmentPlane(size_t pid, size_t plane) {

    LLCV_DEBUG() << "@pid=" << pid << " @plane=" << plane << std::endl;

    switch(pid) {
    case 0 : {
      switch(plane) {
      case 0 : {
	_par_electron_frac = &_par1_electron_frac_U;
	_par_muon_frac     = &_par1_muon_frac_U;
	_par_proton_frac   = &_par1_proton_frac_U;
	break;
      }
      case 1: {
	_par_electron_frac = &_par1_electron_frac_V;
	_par_muon_frac     = &_par1_muon_frac_V;
	_par_proton_frac   = &_par1_proton_frac_V;
	break;
      }
      case 2: {
	_par_electron_frac = &_par1_electron_frac_Y;
	_par_muon_frac     = &_par1_muon_frac_Y;
	_par_proton_frac   = &_par1_proton_frac_Y;
	break;
      }
      default : { break; }
      } // end plane
      break;
    } // end particle 1

    case 1 : {
      switch(plane) {
      case 0 : {
	_par_electron_frac = &_par2_electron_frac_U;
	_par_muon_frac     = &_par2_muon_frac_U;
	_par_proton_frac   = &_par2_proton_frac_U;
	break;
      }
      case 1: {
	_par_electron_frac = &_par2_electron_frac_V;
	_par_muon_frac     = &_par2_muon_frac_V;
	_par_proton_frac   = &_par2_proton_frac_V;
	break;
      }
      case 2: {
	_par_electron_frac = &_par2_electron_frac_Y;
	_par_muon_frac     = &_par2_muon_frac_Y;
	_par_proton_frac   = &_par2_proton_frac_Y;
	break;
      }
      default : { break; }
      } // end plane
      break;
    } // end particle 2
    default : { break; }
    } // end particle

    return;
  }

  float SelNueID::MinimizeToEdge(const larocv::GEO2D_Contour_t& ctor) {
    float res = larocv::kINVALID_FLOAT;
    for(const auto& pt : ctor) {
      res = std::min(res,(float)pt.x);
      res = std::min(res,(float)pt.y);
      res = std::min(res,(float)_cropx - (float)pt.x);
      res = std::min(res,(float)_cropy - (float)pt.y);
    }
    return res;
  }

  float SelNueID::NearestPolygonToCosmic(const std::vector<Polygon>& polygon_v,size_t plane) {
    float ret = larocv::kINVALID_FLOAT;
    const auto& _CosmicTag = _CosmicTag_v[plane];
    for(const auto& polygon : polygon_v) {
      float distance = larocv::kINVALID_FLOAT;
      auto id = _CosmicTag.NearestCosmicToContour(polygon.Contour(),distance);
      ret = std::min(ret,distance);
    }
    return ret;
  }
  
  int SelNueID::DetectBrem(Triangle& triangle, 
			   const larocv::GEO2D_ContourArray_t& other_ctor_v,
			   std::vector<size_t>& id_v,
			   std::vector<size_t>& inside_v) {
    int res = 0;

    //
    // count the number of other contours inside or touching the expanded triangle
    //
    
    float fraction = (triangle.Height() + _brem_dist) / triangle.Height();

    triangle.Extend(fraction);
    
    auto tri_ctor = triangle.AsContour();
    
    id_v.reserve(other_ctor_v.size());
    inside_v.reserve(other_ctor_v.size());

    for(size_t oid=0; oid < other_ctor_v.size(); ++oid) {
      const auto& other_ctor = other_ctor_v[oid];
      
      auto common_area = larocv::AreaOverlap(other_ctor,tri_ctor);
      if (common_area == 0) continue;
      
      int pixel_area = larocv::ContourPixelArea(other_ctor);
      if (pixel_area > _brem_size) {
	res += 1;
	id_v.push_back(oid);
      }
      inside_v.push_back(oid);
    }
    
    return res;
  }
  
  
  void SelNueID::Finalize() {
    LLCV_DEBUG() << "start" << std::endl;
    _outtree->Write();
    LLCV_DEBUG() << "end" << std::endl;
  }

}


#endif
  
