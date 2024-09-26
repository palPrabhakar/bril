#!/bin/sh

# echo Running Dead code elimination 

# FILE=../../test/interp/core/jmp.bril
# FILE=../../test/interp/core/main-bool-args.bril
# FILE=../../test/interp/core/call-with-args.bril
FILE=../../test/interp/core/add-overflow.bril
# FILE=../../examples/test/ssa/loop-orig.bril
#
cat $FILE

if [[ "$#" -eq 1 ]]; then
  FILE=$1
fi


# bril2json < $FILE | ./form-block-bin | bril2txt

bril2json < $FILE | ./cfg-bin

# bril2json < $FILE | python3 ../../examples/cfg_dot.py

