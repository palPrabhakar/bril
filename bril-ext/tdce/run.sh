#!/bin/sh

# echo Running Dead code elimination 

# FILE=../../examples/test/tdce/simple.bril
# FILE=../../examples/test/tdce/skipped.bril
FILE=../../examples/test/tdce/combo.bril

if [[ "$#" -eq 1 ]]; then
  FILE=$1
fi


cat $FILE

bril2json < $FILE | ./tdce-bin | bril2txt

# bril2json < $FILE | ./tdce-bin

bril2json < $FILE | python3 ../../examples/tdce.py tdce+ | bril2txt

