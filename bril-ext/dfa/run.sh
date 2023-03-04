#!/bin/sh

# echo Running Dead code elimination 

# FILE=simple.bril
FILE=../../examples/test/df/cond-args.bril

# FILE=../../examples/test/ssa/loop-orig.bril

if [[ "$#" -eq 1 ]]; then
  FILE=$1
fi


# bril2json < $FILE | ./dfa | bril2txt

./dfa

# cat $FILE
# bril2json < $FILE

bril2json < $FILE | python3 ../../examples/df.py 'defined'

