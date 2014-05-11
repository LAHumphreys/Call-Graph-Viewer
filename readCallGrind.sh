#!/bin/bash

input=$1

if [[ ! -f $input ]]; then
    echo "Usage: readCallGrind.sh <callgrind output file>"
    exit 1
fi

echo -n "Parsing callgrind file..."
cat $input | mawk -f Scripts/split_callgrind.awk
echo "done"

./profile flist.csv calls.csv cost.csv
