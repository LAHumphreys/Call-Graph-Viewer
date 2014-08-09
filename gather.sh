#!/bin/bash

f='gather.out'
rm $f

valgrind --tool=callgrind --branch-sim=yes --separate-recs=10 --separate-callers=20 --callgrind-out-file=$f --cache-sim=yes $@
