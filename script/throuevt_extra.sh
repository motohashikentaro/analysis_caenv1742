#!/bin/bash

for file in ../data/hit/hit_*.root
do
    ../bin/through_event_extractor "$file"

    echo "Through-event extraction completed for $file"
done
