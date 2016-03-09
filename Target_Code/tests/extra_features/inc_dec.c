int main() {
	int a = 10, b = 5;
	int results = 0;

	print "Testing post increment and decrement";

	if (a-- != 10) {
		print "fail 1";
		results++;
	}

	if (a != 9) {
		print "fail 2";
		results++;
	}

	if (a++ != 9) {
		print "fail 3";
		results++;
	}

	if (a != 10) {
		print "fail 4";
		results++;
	}

	a = 5;
	if ( (a++) != (b--) ) {
		print "fail 5";
		results++;
	}

	if (a != 6 || b != 4) {
		print a;
		print b;		
		print "fail 6";
		results++;
	}

	if (results > 0) {
		print "tests failed in increment / decrement";
	} else {
		print "post-increment / decrement tests succeeded!";
	}

	return results;
}