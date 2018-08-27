import ROOT
from larlite import larlite as ll

def InterWriteOut():
    name = "InterWriteOut"
    return ll.InterWriteOut()

def attach_writeout(proc):
    writeout = InterWriteOut()
    proc.add_ll_ana(writeout)

    return
