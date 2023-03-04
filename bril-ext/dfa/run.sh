#!/bin/sh

# echo Running Dead code elimination 

# FILE=simple.bril
# FILE=../../examples/test/df/cond-args.bril
# FILE=../../examples/test/df/cond.bril
FILE=../../examples/test/df/fact.bril


if [[ "$#" -eq 1 ]]; then
  FILE=$1
fi


bril2json < $FILE | ./dfa 

# ./dfa

# cat $FILE
# bril2json < $FILE

bril2json < $FILE | python3 ../../examples/df.py 'defined'

