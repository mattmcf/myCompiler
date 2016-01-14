#!/bin/bash

# FILE: mytest.sh
# AUTHORS: Yondon Fu and Matt McFarland - Delights (CS57 - 16W)
#
# PURPOSE: to make and test the scanner for the compiler
# ATTRIBUTIONS: --

ANS_FILE1=./tests/token_test_ans.txt
SRC_FILE1=./tests/token_test_src.txt

# script
make clean > /dev/null;
make scanner_test

mkdir results

if [ "$?" -ne "0" ]
	then
	echo "errors during make. Fix errors to continue."
	exit 1
fi

echo " "
echo "Testing scanner test with answer key: $ANS_FILE1 and source file: $SRC_FILE1"
./scanner_test $ANS_FILE1 < $SRC_FILE1 > ./results/token_test_results.txt
if [ "$?" -ne "0" ]
	then
	echo "Test Failed."
	exit 1
fi

ANS_FILE2=./tests/print_matrix_tok.txt
SRC_FILE2=./tests/print_matrix.c

echo " "
echo "Testing scanner test with answer key: $ANS_FILE2 and source file: $SRC_FILE2"
./scanner_test $ANS_FILE2 < $SRC_FILE2 > ./results/print_matrix_results.txt
if [ "$?" -ne "0" ]
	then
	echo "Test Failed."
	exit 1
fi	

ANS_FILE3=./tests/hallway_tok.txt
SRC_FILE3=./tests/hallway.c

echo " "
echo "Testing scanner test with answer key: $ANS_FILE3 and source file: $SRC_FILE3"
./scanner_test $ANS_FILE3 < $SRC_FILE3 > ./results/hallways_results.txt
if [ "$?" -ne "0" ]
	then
	echo "Test Failed."
	exit 1
fi

echo " "
echo "All tests succeeded!"
echo "See the results directory for the scanner test output files"
exit 0
