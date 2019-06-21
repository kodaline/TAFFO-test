#!/bin/bash

trap 'echo trapped; kill -s KILL $!; exit 1;' INT

if [[ -z $1 ]]; then
  OUT_DIR='fe-stats'
else
  OUT_DIR=$1
fi

mkdir -p $OUT_DIR

for conf in good bad worse; do
  conf_param=CONF_$(echo $conf | tr '[:lower:]' '[:upper:]')
  echo conf = $conf
  ./compile.sh metrics $conf_param & wait
  ./run.sh --times=20 & wait
  ./validate.py > $OUT_DIR/${conf}.txt & wait
  mv results-out $OUT_DIR/${conf}
done
