import os,sys

if len (sys.argv) != 5:
    print 
    print 
    print "CONFIG_FILE = str(sys.argv[1])"
    print "IMAGE_FILE  = str(sys.argv[2])"
    print "HIT_FILE    = str(sys.argv[3])"
    print "OUTPUT_DIR  = str(sys.argv[4])"
    print 
    sys.exit(1)


CONFIG_FILE = str(sys.argv[1])
IMAGE_FILE  = str(sys.argv[2])
HIT_FILE    = str(sys.argv[3])
OUTPUT_DIR  = str(sys.argv[4])

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

play = llcv.PlayAround()

proc.add_llcv_ana(play)

proc.configure(CONFIG_FILE)
proc.dataco().set_outputfile(os.path.join(OUTPUT_DIR),"larcv")

proc.add_lcv_input_file(IMAGE_FILE)
proc.add_ll_input_file(HIT_FILE)

proc.initialize()

#
# run over larcv entries
#
proc.batch_process_lcv()

proc.finalize()

sys.exit(0)
