#!/bin/bash

# fix awk decimal number parser
export LANG=en_US.UTF-8

rm -rf data/output
mkdir -p data/output
benchmark=sobel
for f in ./../common/img/*.rgb
do
	filename=$(basename "$f")
	extension="${filename##*.}"
	filename="${filename%.*}"

	echo -e "\e[95m------ ${filename} ------\e[0m"
	
	echo -e "\e[96m*** LibVC Version ***\e[0m"
	time ./bin/${benchmark} ${f} data/output/${filename}_${benchmark}.rgb
	
	echo -e "\e[32m### QoS ###\e[0m"
	
	python ./../common/scripts/png2rgb.py png data/output/${filename}_${benchmark}.rgb.baseline data/output/${filename}_${benchmark}.baseline.png > out1.tmp
	python ./../common/scripts/png2rgb.py png data/output/${filename}_${benchmark}.rgb.taffo data/output/${filename}_${benchmark}.taffo.png > out2.tmp
	
	compare -metric RMSE data/output/${filename}_${benchmark}.baseline.png data/output/${filename}_${benchmark}.taffo.png /dev/null > tmp.log 2> tmp.err
	awk '{ printf("*** Relative error: %0.2f%%\n*** Absolute error: %f\n",substr($2, 2, length($2) - 2) * 100,substr($1, 1)) }' tmp.err
done
