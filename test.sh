#!/bin/bash
make
./fujiraw testres/test.RAF
#for filename in res/test/*.RAF; do
#    ./fujiraw "$filename" > "${filename}.meta"
#done
