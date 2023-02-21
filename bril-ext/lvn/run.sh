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


# FILE=../../examples/test/lvn/clobber.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# FILE=../../examples/test/lvn/clobber-fold.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

# FILE=../../examples/test/lvn/clobber-fold.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt


# FILE=../../examples/test/lvn/commute.bril
# bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
# bril2json < $FILE | ./lvn | bril2txt

FILE=../../examples/test/lvn/divide-by-zero.bril
bril2json < $FILE | python3 ../../examples/lvn.py | bril2txt
bril2json < $FILE | ./lvn | bril2txt


