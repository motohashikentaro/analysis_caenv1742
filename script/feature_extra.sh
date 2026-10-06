#!/bin/bash

for file in ../data/root/run_*.root
do
    ../bin/feature_extractor "$file"
done
