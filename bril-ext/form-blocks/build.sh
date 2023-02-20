if [[ -x ./tdce-bin ]]; then
  # echo "make clean"
  make clean
fi

# echo "make call"
# clang++ -std=c++17 main.cpp -o tdce-bin
make all
