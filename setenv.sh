#!/bin/bash -x


taffo_setenv_find()
{
  cd $1
  BASEDIR=$(pwd)
  
  if [[ $2 == 'lib' ]]; then
    SOEXT="so"
    if [ $(uname -s) = "Darwin" ]; then
      SOEXT="dylib";
    fi
    FN="$3.$SOEXT"
  else
    FN="$3"
  fi
  
  PATH="$BASEDIR/$2/$FN"
  
  if [[ ! -e "$PATH" ]]; then
    echo "Cannot find $FN" > /dev/stderr
  else
    echo "Found $PATH" > /dev/stderr
    echo "$PATH"
  fi
}


if [[ $# < 1 ]]; then
  echo 'Usage: source setenv.sh /prefix/where/taffo/was/installed'
  echo 'Install taffo by running make install. The default prefix is /usr/local.'
else
  export INITLIB=$(taffo_setenv_find $1 'lib' 'TaffoInitializer')
  export VRALIB=$(taffo_setenv_find $1 'lib' 'TaffoVRA')
  export TUNERLIB=$(taffo_setenv_find $1 'lib' 'TaffoDTA')
  export PASSLIB=$(taffo_setenv_find $1 'lib' 'LLVMFloatToFixed')
  export ERRORLIB=$(taffo_setenv_find $1 'lib' 'LLVMErrorPropagator')
  export INSTMIX=$(taffo_setenv_find $1 'bin' 'istr_type')
  
  if [[ -z "$LLVM_DIR" ]]; then
    LLVM_DIR=$(llvm-config --prefix 2> /dev/null)
    if [[ $? -ne 0 ]]; then
      printf "*** WARNING ***\nCannot set LLVM_DIR using llvm-config\n"
    fi
  fi
  if [[ ! ( -z "$LLVM_DIR" ) ]]; then
    if [ $(uname -s) = "Darwin" ]; then
      # xcrun patches the command line parameters to clang to add the standard
      # include paths depending on where the currently active platform SDK is
      export CLANG="xcrun $LLVM_DIR/bin/clang"
      export CLANGXX="xcrun $LLVM_DIR/bin/clang++"
    fi
  fi
fi

unset -f taffo_setenv_find
