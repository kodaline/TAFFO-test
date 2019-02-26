#!/bin/bash

SCRIPTPATH=$(dirname "$BASH_SOURCE")

recompile_one() {
  args=
  input=$1
  ext=${1##*.}
  if [[ ( $ext = 'll' ) || ( $(basename $input) = *test* ) ]]; then
    args="$args -c"
  fi
  out=${1%.*}.out
  "$SCRIPTPATH"/magiclang2.sh "$args" -o "$out" "$input" 2> recompile-all.log
  if [[ $? -ne 0 ]]; then
    printf '[FAIL] %s\n' "$input"
    return 1
  else
    printf '[ ok ] %s\n' "$input"
  fi
  return 0
}


for fn in "$SCRIPTPATH"/*.c "$SCRIPTPATH"/*.ll; do
  if [[ ( "$fn" != *.magiclangtmp.ll ) && ( "$fn" != *NOT-WORKING-YET* ) ]]; then
    recompile_one "$fn" || exit $?
  fi
done

