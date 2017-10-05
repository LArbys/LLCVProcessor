import os,sys

if len(sys.argv != 3):
    print 
    print "PGRAPH_FILE = str(sys.argv[1])"
    print "HIT_FILE    = str(sys.argv[2])"
    print 
    sys.exit(1)


PGRAPH_FILE = str(sys.argv[1])
HIT_FILE    = str(sys.argv[2])

num = int(PGRAPH_FILE.split(".")[0].split("_")[-1])

import ROOT
from larlitecv import larlitecv
from ROOT import llcv

BASE_PATH = os.path.realpath(__file__)
BASE_PATH = os.path.dirname(BASE_PATH)
sys.path.insert(0,BASE_PATH)

#
# Configure Processor
#
proc = llcv.Processor()

dlhs = llcv.DLHandshake()
proc.add_llcv_ana(dlhs)

from showerRecoDL import DLShowerReco3D
dlshr3d = DLShowerReco3D()
proc.add_ll_ana(dlshr3d)

proc.configure(os.path.join(BASE_PATH,"config.cfg"))

proc.add_lcv_input_file(PGRAPH_FILE)
proc.add_ll_input_file(HIT_FILE)

proc.set_output_ll_name("shower_reco_out_%d.root" % num)

proc.initialize()

#
# run over larcv entries
#
proc.batch_process_lcv()

proc.finalize()

sys.exit(1)
