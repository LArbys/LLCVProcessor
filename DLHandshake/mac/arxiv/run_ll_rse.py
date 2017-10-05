import os,sys

DIR="../lite_links/links"
data = [os.path.join(DIR,f) for f in os.listdir(DIR) if f.startswith("larlite_reco2d") and f.endswith(".root")]

#data = [data[0]]
from multiprocessing.dummy import Pool as ThreadPool

def work(d):
    num = int(os.path.basename(d).split(".")[0].split("_")[-1])
    print d,num
    SS="python larlite_rse.py %d %s 1>stdout/out_%04d.log 2>stderr/err_%04d.log" %(num,d,num,num)
    print SS
    os.system(SS)
    return

# Make the Pool of workers
pool = ThreadPool(12)

results = pool.map(work, data)

pool.close()
pool.join()
