#!/bin/bash

for file in ../data/root/run_054*old.root
do
    ./feature_extractor "$file"
done