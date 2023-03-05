if [[ -x ./*-bin ]]; then
  # echo "make clean"
  make clean
fi

# echo "make call"
make all
