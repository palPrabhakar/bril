if [[ -x ./tdce-bin ]]; then
  # echo "make clean"
  rm ./tdce-bin
fi

# echo "make call"
clang++ -std=c++17 tdce.cpp -o tdce-bin
