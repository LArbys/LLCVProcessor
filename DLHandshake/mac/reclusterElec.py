import ROOT
from larlite import larlite as fmwk

def ReclusterElec():

    recluster_ana_unit = ElectronRecluster()
    print "Load ElectronRecluster @ ",recluster_ana_unit

    return recluster_ana_unit
