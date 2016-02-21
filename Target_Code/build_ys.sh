#!/bin/bash

# /*
#  * build_ys.c
#  *
#  * USAGE: ./build_ys.c [INPUT FILE] [OUTPUT NAME]
#  *
#  * 1) creates compiler
#  * 2) compiles specified .c file into .ys file
#  * 3) assembles .yo file
#  * 4) initiates y86 graphical simulation
#  */

make clean > /dev/null
make

if [ "$?" -ne 0 ]
  then
  echo "Errors during make. Fix errors to continue."
  exit 1
fi

if [ ! -e "$1" ]
then
	echo "Input file does not exist."
	exit 1
fi

./gen_target_code $2 < $1

if [ "$?" -ne 0 ]
then
	echo "failed to generate target code"
	exit 1
fi

OUT_FILE="$2.ys"
yas $OUT_FILE

if [ "$?" -ne 0 ]
then
	echo "failed to assemble target code"
	exit 1
fi

EXECUTABLE="$2.yo"
ssim -g $EXECUTABLE


