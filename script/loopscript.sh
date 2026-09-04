#!/bin/bash

for ch in {0..31}; do
    ./feature_summary \
        /home/motohashi/work/analysis_caenv1742/data/feature/correct_threshold/feature_05088.root \
        0 \
        "$ch" \
        20
done