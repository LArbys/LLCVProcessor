import os,sys
import ROOT
from larlite import larlite as fmwk1
from larcv import larcv as fmwk2
from ROOT import handshake

hs = handshake.HandShaker()


# data coordinator loop over events?
for lc_entry, ll_entry in zip(self.lc_entries,self.ll_entries):

    
    run    = io1_in.run_id()
    subrun = io1_in.subrun_id()
    event  = io1_in.event_id()
    
    rse = (run,subrun,event)
    print "(rse)=(",run,",",subrun,",",event,")"
    
    this_pres =  self.pres.query("run==@run&subrun==@subrun&event==@event").dropna()
    if this_pres.index.size == 0: 
    print "SKIP"
    continue
    
    io1_out.set_id(run,subrun,event)
    
    #
    # larlite OUT
    #
    ev_pfpart  = io1_out.get_data(fmwk1.data.kPFParticle, "dl")
    ev_vertex  = io1_out.get_data(fmwk1.data.kVertex,     "dl")
    ev_shower  = io1_out.get_data(fmwk1.data.kShower,     "dl")
    ev_track   = io1_out.get_data(fmwk1.data.kTrack,      "dl")
    ev_cluster = io1_out.get_data(fmwk1.data.kCluster,    "dl")
    ev_hit     = io1_out.get_data(fmwk1.data.kHit,        "dl")
    ev_ass     = io1_out.get_data(fmwk1.data.kAssociation,"dl")
    
    # copies
    ev_hit_out     = io1_out.get_data(fmwk1.data.kHit, "gaushit")
    ev_gtruth_out  = io1_out.get_data(fmwk1.data.kGTruth, "generator")
    ev_mctruth_out = io1_out.get_data(fmwk1.data.kMCTruth, "generator")
    ev_shower_out  = io1_out.get_data(fmwk1.data.kMCShower, "mcreco")

    #
    # larlite IN
    #
    ev_hit_in     = io1_in.get_data(fmwk1.data.kHit, "gaushit")
    ev_gtruth_in  = io1_in.get_data(fmwk1.data.kGTruth, "generator")
    ev_mctruth_in = io1_in.get_data(fmwk1.data.kMCTruth, "generator")
    ev_shower_in  = io1_in.get_data(fmwk1.data.kMCShower, "mcreco")

    #
    # larcv IN
    #
    ev_pgraph  = io2.get_data(fmwk2.kProductPGraph,'test')
    ev_pixel2d = io2.get_data(fmwk2.kProductPixel2D,'test_ctor')

    hs.pixel_distance_threshold(1.)
    
    hs.set_larlite_pointers(ev_pfpart, ev_vertex,
                            ev_shower, ev_track,
                            ev_cluster, ev_hit,
                            ev_ass)

    ptypes_v = ROOT.std.vector("int")(3)
    
    #
    # Set particle 1 type
    #
    par1_type = this_pres.par1_type.values[0]
    if par1_type == 1.0: 
        par1_type = 13
    elif par1_type == 2.0:
        par1_type = 11
    else: 
        raise Exception
        
    #
    # Set particle 2 type
    #
    par2_type = this_pres.par2_type.values[0]
    if par2_type == 1.0: 
        par2_type = 13
    elif par2_type == 2.0:
        par2_type = 11
    else:
        raise Exception

    ptypes_v[0] = 12
    ptypes_v[1] = par1_type
    ptypes_v[2] = par2_type
    
    hs.set_pfparticle_types(ptypes_v);

    hs.construct(ev_pgraph, ev_pixel2d, ev_hit_in)
    
    io1_out.next_event()
        
    ctr+=1
    
    print "Closing LL input"
    io1_in.close()
    print "Closing LL output"
    io1_out.close()
    print "Closing LC input"
    io2.finalize()
