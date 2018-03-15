#!/bin/sh

#opreco-Run007577-SubRun000000.root  shower_reco_out_75770000.root               tracker_reco_75770000.root        vertexout_filter_nue_ana_tree_75770000.root
#reco2d-Run007577-SubRun000000.root  ssnetout-larcv-Run007577-SubRun000000.root  track_pgraph_match_75770000.root

datafolder=/home/twongjirad/working/data/larbys/mcc8v6/extbnb_flashmatchsample
#macfile=../../mac/inter_ana_flash.py
macfile=inter_ana_flash.py

ssnet_file=$datafolder/ssnetout-larcv-Run007577-SubRun000000.root
vtx_file=$datafolder/vertexout_filter_numu_ana_tree_75770000.root
flash_file=$datafolder/opreco-Run007577-SubRun000000.root
shower_file=$datafolder/shower_reco_out_75770000.root
track_file=$datafolder/tracker_reco_75770000.root
inter_file=foo.root
out_dir=$PWD/out

mkdir -p $out_dir

cmd="python $macfile $ssnet_file $vtx_file $flash_file $shower_file $track_file"
echo $cmd

python $macfile $ssnet_file $vtx_file $flash_file $shower_file $track_file $inter_file $out_dir

