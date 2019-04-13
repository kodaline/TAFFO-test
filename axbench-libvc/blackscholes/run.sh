#!/bin/bash

# fix awk decimal number parser
export LANG=en_US.UTF-8

rm -rf data/output
mkdir -p data/output
benchmark=blackscholes
for f in data/input/*.data
do
	filename=$(basename "$f")
	extension="${filename##*.}"
	filename="${filename%.*}"


	echo -e "\e[95m------ ${filename} ------\e[0m"

	echo -e "\e[96m*** LibVC Version ***\e[0m"
	time ./bin/${benchmark} ${f} data/output/${filename}_${benchmark}.data
	
	echo -e "\e[32m### QoS ###\e[0m"
	python ./scripts/qos.py data/output/${filename}_${benchmark}.data.baseline data/output/${filename}_${benchmark}.data.taffo

done
