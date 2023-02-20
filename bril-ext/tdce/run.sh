#!/bin/sh

# echo Running Dead code elimination 

FILE=simple.bril

if [[ "$#" -eq 1 ]]; then
  FILE=$1
fi


# bril2json < $FILE | ./tdce-bin | bril2txt

# bril2json < $FILE | ./tdce-bin

bril2json < $FILE | python3 ../../examples/tdce.py tdce+ | bril2txt

