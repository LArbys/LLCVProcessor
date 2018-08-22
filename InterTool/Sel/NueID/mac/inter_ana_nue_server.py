import os,sys,gc

SSNET_FILE = str(sys.argv[1])
VTX_FILE   = str(sys.argv[2])
FLASH_FILE = str(sys.argv[3])
SHR_FILE   = str(sys.argv[4])
TRK_FILE   = str(sys.argv[5])
INTER_FILE = str(sys.argv[6])
IS_MC      = int(str(sys.argv[7]))
OUT_DIR    = str(sys.argv[8])
#EVENT = int(sys.argv[9])

import ROOT
from larlitecv import larlitecv
from ROOT import llcv

BASE_PATH = os.path.realpath(__file__)
BASE_PATH = os.path.dirname(BASE_PATH)
print "Base path at: ",BASE_PATH
sys.path.insert(0,BASE_PATH)

TOP_DIR = os.environ['LARLITECV_BASEDIR']
TOP_DIR = os.path.join(TOP_DIR,"app","LLCVProcessor","InterTool")
sys.path.insert(0,os.path.join(TOP_DIR,"Sel"))

from lib.ssnet_modules import attach_ssnet
from lib.dead_modules import attach_dead

# proc
proc = llcv.Processor()

# attach SSNet
attach_ssnet(proc)

# attach dead wire maker
attach_dead(proc)

# intermodule
imod = llcv.InterModule()

# configure the driver
driver = imod.Driver()

#NUM=1
NUM = int(os.path.basename(VTX_FILE).split(".")[0].split("_")[-1])
driver.SetOutputFilename(os.path.join(OUT_DIR,"nueid_ana_%d.root" % NUM));

selection = llcv.SelNueID()
selection.SetIsMC(IS_MC)
#selection.SetDebug(True)

driver.AddSelection(selection);

# process
proc.add_llcv_ana(imod)

if IS_MC == 1:
    proc.configure(os.path.join(BASE_PATH,"inter_nue_mc.cfg"))
else:
    proc.configure(os.path.join(BASE_PATH,"inter_nue_data.cfg"))

print "ADD=%s"%(SSNET_FILE)
proc.add_lcv_input_file(SSNET_FILE)
print "ADD=%s"%(VTX_FILE)
proc.add_lcv_input_file(VTX_FILE)
print "ADD=%s"%(FLASH_FILE)
proc.add_lcv_input_file(FLASH_FILE)
print "ADD=%s"%(SHR_FILE)
proc.add_lcv_input_file(SHR_FILE)
#proc.add_ll_input_file(TRK_FILE)

#proc.set_output_ll_name(os.path.join(OUT_DIR,"nueid_ll_out_%d.root" % NUM))
proc.dataco().set_outputfile(os.path.join(OUT_DIR, "nueid_lcv_out_%d.root" % NUM),"larcv")

proc.initialize()

#proc.batch_process_lcv_reverse(16,1)
proc.batch_process_lcv_reverse()

proc.finalize()

sys.exit(0)

