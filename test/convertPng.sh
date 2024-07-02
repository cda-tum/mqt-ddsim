#!/bin/bash
for file in BenchmarkQuasm/dot/*.dot; do
    dot "$file" -Tpng -o "${file%.dot}.png"
done
