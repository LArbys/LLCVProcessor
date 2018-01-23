import os,sys,gc

if len(sys.argv) != 6:
    print
    print
    print "SSNET_FILE = str(sys.argv[1])"
    print "VTX_FILE   = str(sys.argv[2])"
    print "SHR_FILE   = str(sys.argv[3])"
    print "TRK_FILE   = str(sys.argv[4])"
    print "OUT_DIR    = str(sys.argv[5])"
    print
    print
    sys.exit(1)

SSNET_FILE = str(sys.argv[1])
VTX_FILE   = str(sys.argv[2])
SHR_FILE   = str(sys.argv[3])
TRK_FILE   = str(sys.argv[4])
OUT_DIR    = str(sys.argv[5])

# NUM = int(SSNET_FILE.split(".")[0].split("_")[-1])

import ROOT
from ROOT import llcv

BASE_PATH = os.path.realpath(__file__)
BASE_PATH = os.path.dirname(BASE_PATH)
sys.path.insert(0,BASE_PATH)

from lib.ssnet_modules import attach_ssnet

proc = llcv.Processor()

# attach ssnet
attach_ssnet(proc)

# run your module

imod = llcv.InterModule()

driver = imod.Driver()

print imod,driver

proc.add_llcv_ana(imod)


#
# process
#
proc.configure(os.path.join(BASE_PATH,"cfg","inter_ana.cfg"))
proc.dataco().set_outputfile(os.path.join(OUT_DIR, "aho.root"),"larcv")

proc.add_lcv_input_file(SSNET_FILE)
proc.add_lcv_input_file(VTX_FILE)
proc.add_ll_input_file(SHR_FILE)
proc.add_ll_input_file(TRK_FILE)

proc.initialize()

proc.batch_process_lcv_reverse()

proc.finalize()

sys.exit(0)

