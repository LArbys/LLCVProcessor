import os, sys
from larlite import larlite as fmwk

INFILE = str(sys.argv[1])
OUTDIR = str(sys.argv[2])
num = int(INFILE.split(".")[0].split("_")[-1])
OUTFILE = os.path.join(OUTDIR,"showerqualsingle_%d.root" % num)

my_proc = fmwk.ana_processor()
my_proc.add_input_file(sys.argv[1])
my_proc.set_io_mode(fmwk.storage_manager.kREAD)
my_proc.set_ana_output_file(OUTFILE)
my_proc.set_output_file("")

sq_module = fmwk.ShowerQuality_nueshowers()
sq_module.SetShowerProducer("showerreco")
sq_module.SetMaxEnergyCut(9e9)
sq_module.SetMinEnergyCut(0.)
sq_module.SetSingleParticleQuality(True)
my_proc.add_process(sq_module)

my_proc.run()

sys.exit(0)

