import ROOT
from larcv import larcv as lcv

def ChannelMax(plane):
    name = "ChannelMaxPlane%d" % plane
    return lcv.ChannelMax(ROOT.std.string(name))

def Combine():
    return lcv.CombineImages("CombineImages")

def Mask(prefix=""):
    return lcv.SegmentMask("%sSegment" % prefix)

def Image(prefix=""):
    return lcv.MaskImage("%sImage" % prefix)


def attach_ssnet(proc):
    # max
    for plane in xrange(3):
        res = ChannelMax(plane)
        proc.add_lc_proc(res)
        
    # combine
    combine = Combine()
    proc.add_lc_proc(combine)
    
    # mask
    shower_mask = Mask("Shower")
    proc.add_lc_proc(shower_mask)

    track_mask = Mask("Track")
    proc.add_lc_proc(track_mask)

    # image
    shower_image = Image("Shower")
    proc.add_lc_proc(shower_image)

    track_image = Image("Track")
    proc.add_lc_proc(track_image)

    
