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
# cat $FILE
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# echo "\n------------\n"

# FILE=../../examples/test/lvn/reassign.bril
# cat $FILE
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# echo "\n------------\n"

# FILE=../../examples/test/lvn/clobber.bril
# # cat $FILE
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# echo "\n------------\n"

# TODO: FIX
FILE=../../examples/test/lvn/commute.bril
# cat $FILE
bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
bril2json < $FILE | ./lvn | bril2txt

# FILE=../../examples/test/lvn/divide-by-zero.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# FILE=../../examples/test/lvn/idchain.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# Constant Folding Section

# echo "\n------------\n"

# FILE=../../examples/test/lvn/redundant-dce.bril
# cat $FILE
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# echo "\n------------\n"

# FILE=../../examples/test/lvn/rename-fold.bril
# cat $FILE
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# echo "\n------------\n"

# FILE=../../examples/test/lvn/nonlocal.bril
# cat $FILE
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# echo "\n------------\n"

# FILE=../../examples/test/lvn/logical-operators.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# echo "\n------------\n"

# FILE=../../examples/test/lvn/fold-comparisons.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# echo "\n------------\n"

# FILE=../../examples/test/lvn/idchain-nonlocal.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# echo "\n------------\n"

# FILE=../../examples/test/lvn/clobber-fold.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt
