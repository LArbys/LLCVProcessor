import os,sys

import root_numpy as rn
import pandas as pd

print "reading lc"
LCDIR="/data/vgenty/1e1p/out_week080717/vertex/ssnet_union/cheater_ssnet_aug11/ana"
lc_list = [pd.DataFrame(rn.root2array(os.path.join(LCDIR,f),treename='ShapeAnalysis')) for f in os.listdir(LCDIR)]
print "...",len(lc_list)

lc_df = pd.concat(lc_list)
lc_df = lc_df.reset_index()
lc_df = lc_df.query("nparticles==2")
lc_df = lc_df.query("par1_type!=par2_type")
lc_df = lc_df[['run','subrun','event','vtxid','roid','par1_type','par2_type']]
lc_df = lc_df.set_index(['run','subrun','event'])
lc_df.to_pickle("particle_type.pkl")
