#!/bin/bash

for file in ./data/root/*.root
do
    ./bin/feature_extraction "$file"
done