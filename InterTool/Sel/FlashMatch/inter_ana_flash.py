import os,sys,gc

if len(sys.argv) != 8:
    print
    print
    print "SSNET_FILE = str(sys.argv[1])"
    print "VTX_FILE   = str(sys.argv[2])"
    print "FLASH_FILE = str(sys.argv[3])"
    print "SHR_FILE   = str(sys.argv[4])"
    print "TRK_FILE   = str(sys.argv[5])"
    print "INTER_FILE = str(sys.argv[6])"
    print "OUT_DIR    = str(sys.argv[7])"
    print
    print
    sys.exit(1)

SSNET_FILE = str(sys.argv[1])
VTX_FILE   = str(sys.argv[2])
FLASH_FILE = str(sys.argv[3])
SHR_FILE   = str(sys.argv[4])
TRK_FILE   = str(sys.argv[5])
INTER_FILE = str(sys.argv[6])
OUT_DIR    = str(sys.argv[7])

# NUM = int(SSNET_FILE.split(".")[0].split("_")[-1])

import ROOT

# weird
from ROOT import flashana
flashana.LightPath()
from ROOT import llcv

BASE_PATH = os.path.realpath(__file__)
BASE_PATH = os.path.dirname(BASE_PATH)
print "Base path at: ",BASE_PATH
sys.path.insert(0,BASE_PATH)

#from lib.ssnet_modules import attach_ssnet

proc = llcv.Processor()

# attach ssnet
# attach_ssnet(proc)

# intermodule
imod = llcv.InterModule()

# configure the driver
driver = imod.Driver()
#driver.AttachInterFile(INTER_FILE,"vertex_tree")
driver.SetOutputFilename("fout.root");

selection = llcv.InterSelFlashMatch()
driver.AddSelection(selection);

# process
proc.add_llcv_ana(imod)

#proc.configure(os.path.join(BASE_PATH,"cfg","inter_flash.cfg"))
proc.configure("inter_flash.cfg")
proc.dataco().set_outputfile(os.path.join(OUT_DIR, "aho.root"),"larcv")

proc.add_lcv_input_file(SSNET_FILE)
proc.add_lcv_input_file(VTX_FILE)
proc.add_ll_input_file(FLASH_FILE)
proc.add_ll_input_file(SHR_FILE)
proc.add_ll_input_file(TRK_FILE)

proc.initialize()

# must start from entry=0, support coming soon
#proc.batch_process_lcv_reverse(0,10)
proc.batch_process_lcv_reverse(0)

proc.finalize()

sys.exit(0)

