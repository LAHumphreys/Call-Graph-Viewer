#!/bin/bash

f='gather.out'
rm $f

valgrind --tool=callgrind --separate-callers=20 --callgrind-out-file=$f $@
