#!/bin/sh

# echo Running Dead code elimination 

# FILE=simple.bril
FILE=../../examples/test/dom/loopcond.bril
# FILE=../../examples/test/dom/while.bril


if [[ "$#" -eq 1 ]]; then
  FILE=$1
fi


bril2json < $FILE | ./dom-bin

# ./dom-bin

# cat $FILE
# bril2json < $FILE

bril2json < $FILE | python3 ../../examples/dom.py 'front'

