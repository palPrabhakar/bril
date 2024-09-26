#!/bin/sh

# echo Running Dead code elimination 

# FILE=../../examples/test/to_ssa/argwrite.bril
# FILE=../../examples/test/to_ssa/if.bril
# FILE=../../examples/test/to_ssa/loop.bril
FILE=../../examples/test/to_ssa/while.bril

# make clean
# make all


# if [[ "$#" -eq 1 ]]; then
#   FILE=$1
# fi


bril2json < $FILE | ./ssa-bin
