#!/bin/bash

# fix awk decimal number parser
export LANG=en_US.UTF-8


if [[ -z $FORMAT ]]; then
  FORMAT='%32s %12s %12s %12s %12s %12s%12s %12s\n'
fi


match_time()
{
  matchstr="$2"
  regex="${matchstr}[^0-9\n]*([0-9.]+)"
  if [[ ( $1 =~ $regex ) ]]; then
    echo ${BASH_REMATCH[1]}
  else
    echo -1
  fi
}


match_error()
{
  regex="^[^${2:0:1}]*$2[^0-9]*([0-9.]+)"
  if [[ ( $1 =~ $regex ) ]]; then
    echo ${BASH_REMATCH[1]}
  else
    echo -1
  fi
}


rm -rf data/output
mkdir -p data/output
benchmark=blackscholes

printf "$FORMAT" "" "T_fix" "T_flt" "T_fix_def" "T_flo_def" "E%" 'EA' 'T_build'

for f in data/input/*.data
do
	filename=$(basename "$f")
	extension="${filename##*.}"
	filename="${filename%.*}"

  if [[ -z $NORUN ]]; then
    res=$(./bin/${benchmark} ${f} data/output/${filename}_${benchmark}.data)
    mfloat=$(match_time "$res" 'baseline version median')
    mfix=$(match_time "$res" 'taffo version median')
    mfloatdef=$(match_time "$res" 'baseline\+define version median')
    mfixdef=$(match_time "$res" 'taffo\+define version median')
    mbuildt=$(match_time "$res" 'compilation time')
  else
    mfloat='0'
    mfix='0'
  fi

  if [[ -z $NOERROR ]]; then
    error=$(./scripts/qos.py data/output/${filename}_${benchmark}.data.baseline data/output/${filename}_${benchmark}.data.taffo)
    mabs_error=$(match_error "$error" 'Absolute error')
    mrel_error=$(match_error "$error" 'Relative error')
  else
    mabs_error='0'
    mrel_error='0'
  fi

  printf "$FORMAT" "${filename}" $mfix $mfloat $mfixdef $mfloatdef "$mrel_error" "$mabs_error" $mbuildt
done
