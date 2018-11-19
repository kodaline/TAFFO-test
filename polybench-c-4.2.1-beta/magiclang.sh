#!/bin/bash -x

ROOT=$(dirname "$0")

if [ $# -lt 1 ]; then
  echo "usage: $0 <filename.c>";
  echo 'compiles a binary but by converting floats to fixed point first';
  echo 'output to filename (without the extension) and to various _tmp* files';
  echo 'any existing files will be overwritten so be careful';
  exit;
fi

if [ -z ${LLVM_DIR+x} ]; then
  echo 'please set $LLVM_DIR to a prefix where LLVM 4.0 can be found';
  exit;
fi

if [ 'x'"$LOG_ONLY" != 'x' ]; then
  exit;
fi

OUTDIR='./build'
mkdir -p $OUTDIR

SOEXT="so"
if [ $(uname -s) = "Darwin" ]; then
  SOEXT="dylib";
fi

if [[ -z "$CLANG" ]]; then CLANG=$LLVM_DIR/bin/clang; fi
if [[ -z "$OPT" ]]; then OPT=$LLVM_DIR/bin/opt; fi
if [[ -z "$LLC" ]]; then LLC=$LLVM_DIR/bin/llc; fi
OUTNAME=$(echo "$5" | sed -E 's/\.[^\.]$//')

ISDEBUG=$("$OPT" --version | grep DEBUG | wc -l)
DEBUGONLYFLAG="-debug-only=flttofix"
if [ $ISDEBUG != '1' ]; then
  DEBUGONLYFLAG='';
fi

$CLANG -S -emit-llvm "$1" -o "$OUTDIR/_$5.tmp0.ll" $3 $4

if [ 'x'$COLLECT_STATS_DIR != 'x' ]; then
  $OPT -load="$PASSLIB" -S -flttofix -dce -stats "$OUTDIR/_$5.tmp0.ll" -o "$OUTDIR/_$5.tmp1.ll" $7 2> "$COLLECT_STATS_DIR/${OUTNAME}.txt"
  echo $(wc -l < "$1") 'LOC' >> "$COLLECT_STATS_DIR/${OUTNAME}.txt"
  $INSTMIX "$OUTDIR/_$5.tmp0.ll" > "$COLLECT_STATS_DIR/${OUTNAME}_ic_float.txt"
  $INSTMIX "$OUTDIR/_$5.tmp1.ll" > "$COLLECT_STATS_DIR/${OUTNAME}_ic_fix.txt"
  exit 0
fi

$OPT -load="$PASSLIB" -pass-remarks=flttofix -S -flttofix -dce $DEBUGONLYFLAG "$OUTDIR/_$5.tmp0.ll" -o "$OUTDIR/_$5.tmp1.ll" $7
$CLANG -S -o "$OUTDIR/_$5.tmp2.s" "$OUTDIR/_$5.tmp1.ll" $2 $3
$CLANG -c -o "$OUTDIR/_$5.tmp2.o" "$OUTDIR/_$5.tmp2.s" $2 $3

if [ 'x'$MIOSIX_TARGET_OBJ == 'x' ]; then
  $CLANG -o "$OUTDIR/$OUTNAME" "$OUTDIR/_$5.tmp2.o" $2 $3 $6
fi

if [ 'x'$DONT_RECOMPILE_FLOAT != 'x' ]; then
  exit 0
fi
$CLANG -S -o "$OUTDIR/_$5.tmp2_not_opt.s" "$OUTDIR/_$5.tmp0.ll" $2 $3
$CLANG -c -o "$OUTDIR/_$5.tmp2_not_opt.o" "$OUTDIR/_$5.tmp2_not_opt.s" $2 $3

if [ 'x'$MIOSIX_TARGET_OBJ == 'x' ]; then
  $CLANG -o "$OUTDIR/$OUTNAME""_not_opt" "$OUTDIR/_$5.tmp2_not_opt.o" $2 $3 $6
fi

if [ 'x'$MIOSIX_TARGET_OBJ != 'x' ]; then
  
  cp "$OUTDIR/_$5.tmp2_not_opt.o" "$MIOSIX_ROOT/$5.o"
  cp "$OUTDIR/_$5.tmp2.o" "$MIOSIX_ROOT/$5.fix.o"
  
  $CLANG -c -o "$OUTDIR/poly.o" "utilities/polybench.c" $2 $3
  cp "$OUTDIR/poly.o" "$MIOSIX_ROOT/poly.o"
fi


