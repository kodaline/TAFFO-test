#!/bin/bash

# fix awk decimal number parser
export LANG=en_US.UTF-8


if [[ -z $FORMAT ]]; then
  FORMAT='%25s %12s %12s %12s %12s %12s%12s %12s\n'
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


rm -rf data/output
mkdir -p data/output
benchmark=sobel

printf "$FORMAT" "" "T_fix" "T_flt" "T_fix_def" "T_flo_def" "E%" 'EA' 'T_build'

for f in ./../common/img/*.rgb
do
  filename=$(basename "$f")
  extension="${filename##*.}"
  filename="${filename%.*}"

  if [[ -z $NORUN ]]; then
    res=$(./bin/${benchmark} ${f} data/output/${filename}_${benchmark}.rgb)
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
    python3 ./../common/scripts/png2rgb.py png data/output/${filename}_${benchmark}.rgb.baseline data/output/${filename}_${benchmark}.baseline.png > out1.tmp
    python3 ./../common/scripts/png2rgb.py png data/output/${filename}_${benchmark}.rgb.taffo data/output/${filename}_${benchmark}.taffo.png > out2.tmp
    
    compare -metric RMSE data/output/${filename}_${benchmark}.baseline.png data/output/${filename}_${benchmark}.taffo.png /dev/null > tmp.log 2> tmp.err
    mrel_error=$(awk '{ printf("%0.6f", substr($2, 2, length($2) - 2) * 100) }' tmp.err)
  else
    mrel_error='0'
  fi
  
  printf "$FORMAT" "${benchmark}_${filename}" $mfix $mfloat $mfixdef $mfloatdef "$mrel_error" '-' $mbuildt
done
