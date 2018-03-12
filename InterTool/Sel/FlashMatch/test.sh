#!/bin/sh


datafolder=/home/twongjirad/working/data/larbys/mcc8v6/goodreco1mu1p
#macfile=../../mac/inter_ana_flash.py
macfile=inter_ana_flash.py

ssnet_file=$datafolder/goodreco_1mu1p_ssnet.root
vtx_file=$datafolder/goodreco_1mu1p_pgraph.root
flash_file=$datafolder/goodreco_1mu1p_opreco.root
shower_file=$datafolder/goodreco_1mu1p_shower.root
track_file=$datafolder/goodreco_1mu1p_track.root
inter_file=foo.root
out_dir=$PWD/out

mkdir -p $out_dir

cmd="python $macfile $ssnet_file $vtx_file $flash_file $shower_file $track_file"
echo $cmd

python $macfile $ssnet_file $vtx_file $flash_file $shower_file $track_file $inter_file $out_dir

