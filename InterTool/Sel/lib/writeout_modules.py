import ROOT
from larlite import larlite as ll
from larcv import larcv as lcv

def LL_InterWriteOut():
    name = "LL_InterWriteOut"
    return ll.InterWriteOut()

def LC_InterWriteOut():
    name = "LC_InterWriteOut"
    return lcv.InterWriteOut(name)

def attach_writeout(proc):
    ll_writeout = LL_InterWriteOut()
    proc.add_ll_ana(ll_writeout)

    lc_writeout = LC_InterWriteOut()
    proc.add_lc_proc(lc_writeout)

    return
