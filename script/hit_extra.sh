#!/bin/bash

for file in ../data/feature/correct_charge/feature_*.root
do
    ../bin/hit_extractor "$file"

    echo "Hit extraction completed for $file"

done
