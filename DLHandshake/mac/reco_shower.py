import os,sys

import ROOT
from larlitecv import larlitecv

from ROOT import llcv

BASE_PATH = os.path.realpath(__file__)
BASE_PATH = os.path.dirname(BASE_PATH)

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

proc.add_lcv_input_file(os.path.join(BASE_PATH,"..","tmp","vertexout_larcv_382133380.root"))
proc.add_ll_input_file(os.path.join(BASE_PATH,"..","tmp","larlite_reco2d_0176.root"))

proc.set_output_ll_name("ll_trash.root")

proc.initialize()

proc.batch_process_lcv()

proc.finalize()

sys.exit(1)
