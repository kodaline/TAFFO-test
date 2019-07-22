#!/bin/bash


# $1 benchmark to check
collect()
{
  pushd $1 > /dev/null
  ./compile+collect.sh "$RESULTS_DIR/axbench"
  popd > /dev/null
}


main()
{
  if [[ -z $1 ]]; then
    RESULTS_DIR="./results"
  else
    RESULTS_DIR="$1"
  fi
  
  if [[ -e "$RESULTS_DIR" ]]; then
    echo 'results dir already exists; archive or remove first please'
    return;
  fi

  rm -r ./raw-times
  mkdir -p "$RESULTS_DIR"
  mkdir -p "$RESULTS_DIR/axbench"
  
  RESULTS_DIR=$(cd ${RESULTS_DIR} 2> /dev/null && pwd -P)

  collect 'blackscholes'
  collect 'blackscholes_00'
  collect 'blackscholes_01'
  collect 'fft'
  collect 'fft_00'
  collect 'fft_01'
  collect 'fft_02'
  collect 'inversek2j'
  collect 'jmeint'
  collect 'kmeans'
  collect 'kmeans_00'
  collect 'kmeans_01'
  collect 'sobel'

  ./chkval_all_better.py 1 > "$RESULTS_DIR/axbench.txt"
  hostname > "$RESULTS_DIR/MACHINE.txt"
}


trap "trap - SIGTERM && kill -- -$$" SIGINT SIGTERM
main "$1" & wait

