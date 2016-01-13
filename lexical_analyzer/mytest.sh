#!/bin/bash

ANS_FILE=./tests/token_test_ans.txt
SRC_FILE=./tests/token_test_src.txt

# script
make scanner_test

if [ -e ./scanner_test ] 
	then
	./scanner_test $ANS_FILE < $SRC_FILE
fi
	
