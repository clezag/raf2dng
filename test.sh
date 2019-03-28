#!/bin/bash
make
./fujiraw res/test.RAF
#for filename in res/test/*.RAF; do
#    ./fujiraw "$filename" > "${filename}.meta"
#done
