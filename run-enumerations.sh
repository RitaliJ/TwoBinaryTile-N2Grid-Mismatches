#!/bin/bash

# Enable nullglob so that non-matching globs expand to null instead of themselves
shopt -s nullglob

# Process each .txt file in the data directory with mismatch-solver-two-per-pos.o
for file in ./data/*.txt; do
    ./out/mismatch-solver-two-per-pos.o "$file" 5
done

# Process each .bin file in the output directory with mismatch-statistics.o
for binfile in ./output/*.bin; do
    ./out/mismatch-statistics.o "$binfile" 5.43
done