int global_a = 5;
int global_b = 9;

int main(void) {
	int a = 0;
	int b = 10;
	int c;
	int g = 5;
	int d;
	int e;
	int f = 5;
	int h[12];

	print "Feed me a number!";
	read c;

	print c;

	print "Should print 2 WOOs";

	while (b > 8 && a < 10) {
		print "WOO!";
		++a;
		--b;
	}

	print "DONE!";

	print "Should print 4 YAYs";

	for (c = 0; c < 4; ++c) {
		print "YAY!";
	}

	print "DONE AGAIN!";

	print "Should print 6 WAHOOs";

	do {
		print "WAHOO!";
		--g;
	} while (g >= 0);

	print "DONE YET AGAIN!";

	// Increment and decrement operator tests
	d = f--;

	print "d after post decrement. Expected 5";
	print d; // d = 5
	print "f after post decrement. Expected 4";
	print f; // f = 4

	e = --f;

	print "e after pre decrement. Expected 3";
	print e; // e = 3
	print "f after pre decrement. Expected 3";
	print f; // f = 3

	// Size of tests
	print "SIZEOF c (int). Expected 4";
	d = sizeof(c); 
	print d; // 4

	print "SIZEOF h (array). Expected 48";
	d = sizeof(h);
	print d; // 48

	return 0;
}