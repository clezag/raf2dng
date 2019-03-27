#!/bin/bash
make
for filename in res/test/*.RAF; do
    ./fujiraw "$filename" > "${filename}.meta"
done
