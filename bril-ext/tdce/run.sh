#!/bin/sh

# echo Running Dead code elimination 

FILE=simple.bril

if [[ "$#" -eq 1 ]]; then
  FILE=$1
fi

if [[ -x ./tdce-bin ]]; then
  # echo "make clean"
  rm ./tdce-bin
fi

# echo "make call"
clang++ -std=c++17 tdce.cpp -o tdce-bin

bril2json < $FILE | ./tdce-bin | bril2txt

