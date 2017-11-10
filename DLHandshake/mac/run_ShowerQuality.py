import os, sys

if len(sys.argv) != 3:
    print 
    print "INFILE  = str(sys.argv[1])"
    print "OUTDIR = str(sys.argv[2])"
    print 
    sys.exit(1)

from larlite import larlite as fmwk

INFILE = str(sys.argv[1])
OUTDIR = str(sys.argv[2])
num = int(INFILE.split(".")[0].split("_")[-1])
OUTFILE = os.path.join(OUTDIR,"showerqualsingle_%d.root" % num)

my_proc = fmwk.ana_processor()
my_proc.add_input_file(INFILE)
my_proc.set_io_mode(fmwk.storage_manager.kREAD)
my_proc.set_ana_output_file(OUTFILE)
my_proc.set_output_file("")

sq_module = fmwk.ShowerQuality_DL()
sq_module.SetVertexProducer("dl")
sq_module.SetShowerProducer("showerreco")
my_proc.add_process(sq_module)

my_proc.run()

sys.exit(0)
