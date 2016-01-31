#!/bin/bash

# FILE: mytest.sh
# AUTHORS: Yondon Fu and Matt McFarland - Delights (CS57 - 16W)
#
# PURPOSE: to make and test the symbol table for the compiler
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

SRC_FILE1=/tests/tcurly.c
ANS_FILE1=/tests/tcurly_ans.txt

./symtab < tests/tcurly.c > results/tcurly.out

echo " "
echo "Symbol table test with source file: $SRC_FILE1 and answer key: $ANS_FILE1"
echo "Diffing answer key and output file..."

diff --unchanged-line-format="" --new-line-format=":%dn: %L" tests/tcurly_ans.txt results/tcurly.out

if [ "$?" -ne "0" ]
  then
  echo "Test Failed."
  exit 1
fi

echo " "
echo "All tests suceeded!"
echo "See the results directory for the symbol table test output files"
exit 0
