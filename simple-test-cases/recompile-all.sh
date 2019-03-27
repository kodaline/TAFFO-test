#!/bin/bash

SCRIPTPATH=$(dirname "$BASH_SOURCE")

recompile_one() {
  args=
  input=$1
  ext=${1##*.}
  if [[ ( $ext = 'll' ) || ( $(basename $input) = test* ) ]]; then
    args="$args -c"
  fi
  extraargs=
  extraargspatt=
  if [[ ( $ext = 'c' ) || ( $ext = 'cpp' ) ]]; then
    extraargspatt='///TAFFO_TEST_ARGS'
  elif [[ $ext = 'll' ]]; then
    extraargspatt=';;;TAFFO_TEST_ARGS'
  fi
  if [[ ! ( -z "$extraargspatt" ) ]]; then
    argstmp=$(grep -E -m 1 "$extraargspatt" $input)
    if [[ ! ( -z "$argstmp" ) ]]; then
      extraargs=${argstmp/$extraargspatt/}
    fi
  fi
  out=${1%.*}.out
  "$SCRIPTPATH"/magiclang2.sh "$args" -o "$out" "$input" $extraargs -debug 2> "$input".log
  if [[ $? -ne 0 ]]; then
    printf '[FAIL] %s\n' "$input"
    return 0
  else
    printf '[ ok ] %s\n' "$input"
  fi
  return 0
}

if [[ "$1" == "clean" ]]; then
  rm "$SCRIPTPATH"/*.magiclangtmp.*
  rm "$SCRIPTPATH"/*.out
  rm "$SCRIPTPATH"/*.log
  exit 0
fi

for fn in "$SCRIPTPATH"/*.c "$SCRIPTPATH"/*.cpp "$SCRIPTPATH"/*.ll; do
  if [[ ( "$fn" != *.magiclangtmp.ll ) && ( "$fn" != *NOT-WORKING-YET* ) ]]; then
    recompile_one "$fn" || exit $?
  fi
done

