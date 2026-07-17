#!/bin/bash

for file in ../data/root/*.root
do
    ./hit_extraction "$file"
done