#!/bin/sh

echo Running Dead code elimination 

FILE=simple.bril

if [[ "$#" -eq 1 ]]; then
  FILE=$1
fi

if [[ -x ./tdce-bin ]]; then
  echo "make clean"
  make clean
fi

echo "make call"
make all

bril2json < $FILE | ./tdce-bin

