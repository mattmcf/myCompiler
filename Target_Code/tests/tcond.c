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

	read c;

	print c;

	while (b > 8 && a < 10) {
		print "WOO!";
		++a;
		--b;
	}

	print "DONE!";

	for (c = 0; c < 4; ++c) {
		print "YAY!";
	}

	print "DONE AGAIN!";

	do {
		print "WAHOO!";
		--g;
	} while (g >= 0);

	print "DONE YET AGAIN!";

	// Increment and decrement operator tests
	d = f--;

	print d; // d = 5
	print f; // f = 4

	e = --f;

	print e; // e = 3
	print f; // f = 3

	return 0;
}