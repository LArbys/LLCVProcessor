import os,sys

import root_numpy as rn
import pandas as pd


#
# Correlate via RSE
#
print "reading lc"
LCDIR="lc_ana"
lc_list = [pd.DataFrame(rn.root2array(os.path.join(LCDIR,f),treename='RSEFilter')) for f in os.listdir(LCDIR)]
print "...",len(lc_list)

print "reading ll"
LLDIR="ll_ana"
ll_list = [pd.DataFrame(rn.root2array(os.path.join(LLDIR,f),treename='ll_rse')) for f in os.listdir(LLDIR)]
print "...",len(ll_list)
print
print "...done"

lc_df = lc_list.pop()
for df in lc_list: 
    lc_df = lc_df.append(df)


ll_df = ll_list.pop()
for df in ll_list: 
    ll_df = ll_df.append(df)


lc_df['lc_entry'] = lc_df.index.values
ll_df['ll_entry'] = ll_df.index.values

rse = ['run','subrun','event']

lc_df = lc_df.reset_index().set_index(rse)
ll_df = ll_df.reset_index().set_index(rse)

lc_df = lc_df.rename(columns = {'fname':'lc_fname'})
ll_df = ll_df.rename(columns = {'fname':'ll_fname'})

def doit(x) : 
    return int(os.path.basename(x).split(".")[0].split("_")[-1])
lc_df['lc_num'] = lc_df['lc_fname'].apply(lambda x : doit(x))
ll_df['ll_num'] = ll_df['ll_fname'].apply(lambda x : doit(x))

print "lc index",lc_df.index.size
print "ll index",ll_df.index.size

res = pd.concat([lc_df,ll_df],axis=1).dropna()
print res.index.size
res.to_pickle("correlated.pkl")



