#!/bin/sh

# echo Running Dead code elimination 

# FILE=../../examples/test/tdce/simple.bril
FILE=../../examples/test/tdce/diamond.bril

if [[ "$#" -eq 1 ]]; then
  FILE=$1
fi


# bril2json < $FILE | ./form-block-bin | bril2txt

bril2json < $FILE | ./form-block-bin

# bril2json < $FILE | python3 ../../examples/tdce.py tdce+ | bril2txt

