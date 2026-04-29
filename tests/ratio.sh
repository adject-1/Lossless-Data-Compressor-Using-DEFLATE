#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: ./ratio.sh <original> <compressed>"
    exit 1
fi

original=$(wc -c < "$1")
compressed=$(wc -c < "$2")

python3 -c "
original = $original
compressed = $compressed
print(f'Original:    {original} bytes')
print(f'Compressed:  {compressed} bytes')
print(f'Ratio:       {original/compressed:.2f}x')
print(f'Space saved: {(1 - compressed/original)*100:.1f}%')
"
