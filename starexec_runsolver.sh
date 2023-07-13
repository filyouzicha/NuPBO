#!/bin/bash

instance=$1
seed=$2
wl=$3

./runsolver --timestamp -o output.out -v output.var -w output.wat -C $wl -W $wl ./source_code/NuPBO $instance $seed
cat output.out
rm -f output.out
rm -f output.var
rm -f output.wat