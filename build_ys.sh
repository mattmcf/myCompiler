#!/bin/bash

# /*
#  * build_ys.sh
#  *
#  * USAGE: ./build_ys.sh [INPUT FILE] [OUTPUT NAME] [optional: '-g']
#  *
#  * 1) creates compiler
#  * 2) compiles specified .c file into .ys file
#  * 3) assembles .yo file
#  * 4) initiates y86 (if '-g' is given, then invokes graphical simulator)
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
	echo "Failed to generate target code"
	make clean > /dev/null
	exit 1
fi

if [ -z "$2" ]
then
	$2="myfile"
fi

OUT_FILE="$2.ys"
yas $OUT_FILE

if [ "$?" -ne 0 ]
then
	echo "Failed to assemble target code"
	exit 1
fi

EXECUTABLE="$2.yo"
echo "Running .yo executable $EXECUTABLE"
ssim $3 $EXECUTABLE


