#!/bin/bash

# FILE: mytest.sh
# AUTHORS: Yondon Fu and Matt McFarland - Delights (CS57 - 16W)
#
# Purpose: bash script file to make and test type checking for the compiler
# ATTRIBUTIONS: --



# Make
make clean > /dev/null
make

mkdir results

if [ "$?" -ne 0 ]
  then
  echo "Errors during make. Fix errors to continue."
  exit 1
fi

SRC_FILE1=/tests/tfunc.c
ANS_FILE1=/tests/tfunc_ans.txt

./IR_gen < tests/tfunc.c > results/tfunc.out

echo " "
echo "IR generating with source file: $SRC_FILE1 and answer key: $ANS_FILE1"
echo "Diffing answer key and output file..."

diff --unchanged-line-format="" --new-line-format=":%dn: %L" tests/tfunc_ans.txt results/tfunc.out

if [ "$?" -ne "0" ]
  then
  echo "Test Failed."
  exit 1
fi

SRC_FILE2=/tests/tarr.c
ANS_FILE2=/tests/tarr_ans.txt

./IR_gen < tests/tarr.c > results/tarr.out

echo " "
echo "IR generating with source file: $SRC_FILE2 and answer key: $ANS_FILE2"
echo "Diffing answer key and output file..."

diff --unchanged-line-format="" --new-line-format=":%dn: %L" tests/tarr_ans.txt results/tarr.out

if [ "$?" -ne "0" ]
  then
  echo "Test Failed."
  exit 1
fi

SRC_FILE3=/tests/test_simple.c
ANS_FILE3=/tests/test_simple_ans.txt

./IR_gen < tests/test_simple.c > results/test_simple.out

echo " "
echo "IR generating with source file: $SRC_FILE3 and answer key: $ANS_FILE3"
echo "Diffing answer key and output file..."

diff --unchanged-line-format="" --new-line-format=":%dn: %L" tests/test_simple_ans.txt results/test_simple.out

if [ "$?" -ne "0" ]
  then
  echo "Test Failed."
  exit 1
fi

SRC_FILE4=/tests/tops.c
ANS_FILE4=/tests/tops_ans.txt

./IR_gen < tests/tops.c > results/tops.out

echo " "
echo "IR generating with source file: $SRC_FILE4 and answer key: $ANS_FILE4"
echo "Diffing answer key and output file..."

diff --unchanged-line-format="" --new-line-format=":%dn: %L" tests/tops_ans.txt results/tops.out

if [ "$?" -ne "0" ]
  then
  echo "Test Failed."
  exit 1
fi

SRC_FILE5=/tests/tloops.c
ANS_FILE5=/tests/tloops_ans.txt

./IR_gen < tests/tloops.c > results/tloops.out

echo " "
echo "IR generating with source file: $SRC_FILE5 and answer key: $ANS_FILE5"
echo "Diffing answer key and output file..."

diff --unchanged-line-format="" --new-line-format=":%dn: %L" tests/tloops_ans.txt results/tloops.out

if [ "$?" -ne "0" ]
  then
  echo "Test Failed."
  exit 1
fi

echo " "
echo "All tests suceeded!"
echo "See the results directory for the symbol table test output files"
exit 0
