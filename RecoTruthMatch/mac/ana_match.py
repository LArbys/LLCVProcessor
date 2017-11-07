import os,sys

if len(sys.argv) != 4:
    print 
    print "SSNET_FILE  = str(sys.argv[1])"
    print "TRACK_FILE  = str(sys.argv[2])"
    print "OUTPUT_DIR  = str(sys.argv[3])"
    print 
    sys.exit(1)


SSNET_FILE = str(sys.argv[1])
TRACK_FILE = str(sys.argv[2])
OUTPUT_DIR = str(sys.argv[3])

num = int(os.path.basename(SSNET_FILE).split(".")[0].split("_")[-1])

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

track_match = llcv.TrackTruthMatch()
proc.add_llcv_ana(track_match)

proc.configure(os.path.join(BASE_PATH,"config.cfg"))
proc.dataco().set_outputfile(os.path.join(OUTPUT_DIR,"track_truth_match_%d.root" % num),"larcv")
proc.add_lcv_input_file(SSNET_FILE)
proc.add_ll_input_file(TRACK_FILE)
proc.initialize()

#
# run over larcv entries
#
proc.batch_process_lcv()

proc.finalize()

sys.exit(0)
