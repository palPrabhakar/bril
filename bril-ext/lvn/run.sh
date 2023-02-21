#!/bin/sh

# echo Running Dead code elimination 

FILE=../../examples/test/lvn/redundant.bril
# FILE=../../examples/test/lvn/reassign.bril

if [[ "$#" -eq 1 ]]; then
  FILE=$1
fi


# bril2json < $FILE | ./lvn | bril2txt

# bril2json < $FILE | ./lvn

bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt

# bril2json < $FILE | python3 ../../examples/lvn.py 

