from handshake import HandShake
import pandas as pd
import ROOT

from multiprocessing.dummy import Pool as ThreadPool

def work(d):

    name  = d[0]
    group = d[1]

    lc_num, ll_num = name

    lc_name_v = pd.unique(group['lc_fname'])
    assert lc_name_v.size == 1

    ll_name_v = pd.unique(group['ll_fname'])
    assert ll_name_v.size == 1

    lc_name_v = lc_name_v[0]
    ll_name_v = ll_name_v[0]
    
    # if lc_num != 160055666: return
    # if ll_num != 19: return

    lc_entry_v = group['lc_entry']
    ll_entry_v = group['ll_entry']
    
    hs = HandShake(lc_name_v,lc_num,lc_entry_v,
                   ll_name_v,ll_num,ll_entry_v)

    hs.set_pgraph_file("/home/vgenty/1e1p/out_week080717/vertex/ssnet_union/cheater_true_aug11/out")

    pres = pd.read_pickle("particle_type.pkl")
    pres = pres.ix[group.index]
    hs.set_particle_data(pres)

    hs.set_output_file("handshook/handshake_%d_%04d.root" % (lc_num,ll_num) )
    print "hs.run()"
    hs.run()
    
    del hs

res = pd.read_pickle("correlated.pkl")

data = []
for name, group in res.groupby(["lc_num","ll_num"]):
    data.append((name,group))

# Make the Pool of workers
pool = ThreadPool(12)

results = pool.map(work, data)

pool.close()
pool.join()


