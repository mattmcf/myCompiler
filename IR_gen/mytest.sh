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

./check_symbols < tests/tfunc.c > results/tfunc.out

echo " "
echo "Type checking test with source file: $SRC_FILE1 and answer key: $ANS_FILE1"
echo "Diffing answer key and output file..."

diff --unchanged-line-format="" --new-line-format=":%dn: %L" tests/tfunc_ans.txt results/tfunc.out

if [ "$?" -ne "0" ]
  then
  echo "Test Failed."
  exit 1
fi

SRC_FILE2=/tests/tscope.c
ANS_FILE2=/tests/tscope_ans.txt

./check_symbols < tests/tscope.c > results/tscope.out

echo " "
echo "Type checking test with source file: $SRC_FILE2 and answer key: $ANS_FILE2"
echo "Diffing answer key and output file..."

diff --unchanged-line-format="" --new-line-format=":%dn: %L" tests/tscope_ans.txt results/tscope.out

if [ "$?" -ne "0" ]
  then
  echo "Test Failed."
  exit 1
fi

SRC_FILE3=/tests/ttypecheck.c
SRC_FILE3=/tests/ttypecheck_ans.txt

./check_symbols < tests/ttypecheck.c > results/ttypecheck.out

echo " "
echo "Type checking test with source file: $SRC_FILE3 and answer key: $ANS_FILE3"
echo "Diffing answer key and output file..."

diff --unchanged-line-format="" --new-line-format=":%dn: %L" tests/ttypecheck_ans.txt results/ttypecheck.out

if [ "$?" -ne "0" ]
  then
  echo "Test Failed."
  exit 1
fi

echo " "
echo "All tests suceeded!"
echo "See the results directory for the symbol table test output files"
exit 0
