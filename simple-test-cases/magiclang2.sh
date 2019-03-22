#!/bin/bash

if [[ -z $PASSLIB ]]; then
  echo -e '\033[31m'"Error"'\033[39m'" please set PASSLIB to the location of LLVMFloatToFixed.so";
fi
if [[ -z $LLVM_DIR ]]; then
  echo -e '\033[33m'"Warning"'\033[39m'" using default llvm/clang";
else
  llvmbin="$LLVM_DIR/bin/";
fi
if [[ -z "$CLANG" ]]; then CLANG=${llvmbin}clang; fi
if [[ -z "$CLANGXX" ]]; then CLANGXX=${CLANG}++; fi
if [[ -z "$OPT" ]]; then OPT=${llvmbin}opt; fi
if [[ -z "$LLC" ]]; then LLC=${llvmbin}llc; fi

parse_state=0
raw_opts="$@"
input_files=
output_file="a"
optimization=
opts=
init_flags=
vra_flags=
disable_vra=0
dta_flags=
conversion_flags=
dontlink=
iscpp=$CLANG
for opt in $raw_opts; do
  case $parse_state in
    0)
      case $opt in
        -Xinit)
          parse_state=2;
          ;;
        -Xvra)
          parse_state=5;
          ;;
        -Xdta)
          parse_state=3;
          ;;
        -Xconversion)
          parse_state=4;
          ;;
        -o*)
          if [[ ${#opt} -eq 2 ]]; then
            parse_state=1;
          else
            output_file=`echo "$opt" | cut -b 2`;
          fi;
          ;;
        -O*)
          optimization=$opt;
          ;;
        -fixp*)
          conversion_flags="$conversion_flags $opt";
          ;;
        -c)
          dontlink="$opt";
          ;;
        -debug)
          init_flags="$init_flags -debug";
          dta_flags="$dta_flags -debug";
          conversion_flags="$conversion_flags -debug";
          vra_flags="$vra_flags -debug";
          ;;
        -debug-taffo)
          init_flags="$init_flags --debug-only=taffo-init";
          dta_flags="$dta_flags --debug-only=taffo-dta";
          conversion_flags="$conversion_flags --debug-only=taffo-conversion";
          vra_flags="$vra_flags --debug-only=taffo-vra";
          ;;
        -disable-vra)
          disable_vra=1
          init_flags="$init_flags -vracompat";
          ;;
        -*)
          opts="$opts $opt";
          ;;
        *.c | *.ll)
          input_files="$input_files $opt";
          ;;
        *.cpp | *.cc)
          input_files="$input_files $opt";
          iscpp=$CLANGXX
          ;;
        *)
          opts="$opts $opt";
          ;;
      esac;
      ;;
    1)
      output_file="$opt";
      parse_state=0;
      ;;
    2)
      init_flags="$init_flags $opt";
      parse_state=0;
      ;;
    3)
      dta_flags="$dta_flags $opt";
      parse_state=0;
      ;;
    4)
      conversion_flags="$conversion_flags $opt";
      parse_state=0;
      ;;
    5)
      vra_flags="$vra_flags $opt";
      parse_state=0;
      ;;
  esac;
done

set -x

${CLANG} \
  $opts -O0 \
  -c -emit-llvm \
  ${input_files} \
  -S -o "${output_file}.1.magiclangtmp.ll" || exit $?
${OPT} \
  -load "$INITLIB" \
  -taffoinit \
  ${init_flags} \
  -S -o "${output_file}.2.magiclangtmp.ll" "${output_file}.1.magiclangtmp.ll" || exit $?
if [[ $disable_vra -eq 0 ]]; then
  ${OPT} \
    -load "$VRALIB" \
    -taffoVRA \
    ${vra_flags} \
    -S -o "${output_file}.3.magiclangtmp.ll" "${output_file}.2.magiclangtmp.ll" || exit $?;
else
  cp "${output_file}.2.magiclangtmp.ll" "${output_file}.3.magiclangtmp.ll";
fi
${OPT} \
  -load "$TUNERLIB" \
  -taffodta \
  ${dta_flags} \
  -S -o "${output_file}.4.magiclangtmp.ll" "${output_file}.3.magiclangtmp.ll" || exit $?
${OPT} \
  -load "$PASSLIB" \
  -flttofix -dce \
  ${conversion_flags} \
  -S -o "${output_file}.5.magiclangtmp.ll" "${output_file}.4.magiclangtmp.ll" || exit $?
${CLANG} \
  $opts ${optimization} \
  -c \
  "${output_file}.5.magiclangtmp.ll" \
  -S -o "$output_file.magiclangtmp.s" || exit $?
${iscpp} \
  $opts ${optimization} \
  ${dontlink} \
  "${output_file}.5.magiclangtmp.ll" \
  -o "$output_file" || exit $?


