#!/bin/bash

DAT2ROOT="$HOME/work/lgad/analysis_caenv1742/external/v1742daq4kek/decoder_only/dat2root"
INPUT_DIR="$HOME/data/kek_202605_sorted"
OUTPUT_DIR="$HOME/work/lgad/analysis_caenv1742/data/root"

mkdir -p "$OUTPUT_DIR"

for input_file in "$INPUT_DIR"/run_*.dat; do
    echo "Processing: $input_file"

    "$DAT2ROOT" "$input_file" "$OUTPUT_DIR"

    if [ $? -ne 0 ]; then
        echo "ERROR: failed to process $input_file"
    fi
done

echo "Finished."
