#!/bin/sh

# echo Running Dead code elimination 

# FILE=../../examples/test/lvn/redundant.bril
# FILE=../../examples/test/lvn/reassign.bril
# FILE=../../examples/test/lvn/clobber-fold.bril

# if [[ "$#" -eq 1 ]]; then
#   FILE=$1
# fi

# bril2json < $FILE | ./lvn | bril2txt
# bril2json < $FILE | ./lvn
# bril2json < $FILE | python3 ../../examples/lvn.py 
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt

# FILE=../../examples/test/lvn/redundant.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# echo "\n------------\n"

# FILE=../../examples/test/lvn/reassign.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# echo "\n------------\n"

# FILE=../../examples/test/lvn/clobber.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# echo "\n------------\n"

# FILE=../../examples/test/lvn/clobber-fold.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# FILE=../../examples/test/lvn/commute.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# FILE=../../examples/test/lvn/divide-by-zero.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# FILE=../../examples/test/lvn/idchain-prop.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# FILE=../../examples/test/lvn/idchain.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

FILE=../../examples/test/lvn/redundant-dce.bril
bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
bril2json < $FILE | ./lvn | bril2txt

# FILE=../../examples/test/lvn/rename-fold.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# NOTE --> This case fails
# FILE=../../examples/test/lvn/nonlocal.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# NOTE --> This case fails
# FILE=../../examples/test/lvn/logical-operators.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# NOTE --> This case fails
# The algo doesn't work on anything other than int types lol
# FILE=../../examples/test/lvn/fold-comparisons.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# NOTE --> This case fails
# Fails with jmp cases
# FILE=../../examples/test/lvn/idchain-nonlocal.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt


