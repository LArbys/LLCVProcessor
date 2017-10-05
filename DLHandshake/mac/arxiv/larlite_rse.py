import sys
from larlite import larlite as ll
my_proc = ll.ana_processor()
my_proc.add_input_file(sys.argv[2])
my_proc.set_io_mode(ll.storage_manager.kREAD)
my_proc.set_ana_output_file("ll_ana/ll_ana_%06d.root" % int(sys.argv[1]));
my_proc.add_process(ll.rse())
my_proc.run();
sys.exit(0)
