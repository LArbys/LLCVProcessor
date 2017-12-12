import os,sys

if len(sys.argv) != 4:
    print 
    print "CLUSTER_FILE = str(sys.argv[1])"
    print "HIT_FILE     = str(sys.argv[2])"
    print "OUTPUT_DIR   = str(sys.argv[3])"
    print 
    sys.exit(1)

CLUSTER_FILE = str(sys.argv[1])
HIT_FILE     = str(sys.argv[2])
OUTPUT_DIR   = str(sys.argv[3])

num = int(os.path.basename(CLUSTER_FILE).split(".")[0].split("_")[-1])

import ROOT
from larlitecv import larlitecv
from ROOT import llcv
import reclusterElec

BASE_PATH = os.path.realpath(__file__)
BASE_PATH = os.path.dirname(BASE_PATH)
sys.path.insert(0,BASE_PATH)

#
# Configure Processor
#
proc = llcv.Processor()

# make the raw cluster
raw_cluster = reclusterElec.ClusterRaw()
proc.add_ll_ana(raw_cluster)

# correlate raw to dl
dlraw = reclusterElec.CorrelateDLRaw()
proc.add_ll_ana(dlraw)

# recluster seeds
recluster = reclusterElec.Merge()
proc.add_ll_ana(recluster)

proc.configure(os.path.join(BASE_PATH,"config_recluster.cfg"))
proc.add_ll_input_file(CLUSTER_FILE)
proc.add_ll_input_file(HIT_FILE)

proc.set_output_ll_name(os.path.join(OUTPUT_DIR,"recluster_reco_out_%d.root" % num))
proc.initialize()

#
# run over larcv entries
#
proc.batch_process_ll(13,1)

proc.finalize()

sys.exit(0)
