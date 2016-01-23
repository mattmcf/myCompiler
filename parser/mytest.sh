#!/bin/bash

# FILE: mytest.sh
# AUTHORS: Yondon Fu and Matt McFarland - Delights (CS57 - 16W)
#
# PURPOSE: to make and test the parser for the compiler
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

SRC_FILE1=/tests/t1.c
ANS_FILE1=/tests/t1.key

./parser < tests/t1.c > results/t1.out

echo " "
echo "Parser test with source file: $SRC_FILE1 and answer key: $ANS_FILE1"
echo "Diffing answer key and output file..."

diff --unchanged-line-format="" --new-line-format=":%dn: %L" tests/t1.key results/t1.out

if [ "$?" -ne "0" ]
  then
  echo "Test Failed."
  exit 1
fi  

SRC_FILE2=/tests/test_parse.c
ANS_FIILE2=/tests/test_parse.key

./parser < tests/test_parse.c > results/test_parse.out

echo " "
echo "Parser test with source file: $SRC_FILE2 and answer key: $ANS_FILE2"
echo "Diffing answer key and output file..."

diff --unchanged-line-format="" --new-line-format=":%dn: %L" tests/test_parse.key results/test_parse.out

if [ "$?" -ne "0" ]
  then
  echo "Test Failed."
  exit 1
fi 

echo " "
echo "All tests suceeded!"
echo "See the results directory for the parser test output files"
exit 0
