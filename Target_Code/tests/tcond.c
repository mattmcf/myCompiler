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

	// while (a < 10) {
	// 	int hello = 2;

	// 	if (hello == 2) {
	// 		break;
	// 	}
	// }

	// print "Broke out of loop!";

	for (e = 0; e < 2; ++e) {
		for (d = 0; d < 10; ++d) {
			if (d == 2) {
				break;
			}

			print "Inner:";
			print d;
		}

		print "Outer:";
		print e;
	}

	// print "Feed me a number!";
	// read c;

	// print c;

	// while (b > 8 && a < 10) {
	// 	print "WOO!";
	// 	++a;
	// 	--b;
	// }

	// print "DONE!";

	// for (c = 0; c < 4; ++c) {
	// 	print "YAY!";
	// }

	// print "DONE AGAIN!";

	// do {
	// 	print "WAHOO!";
	// 	--g;
	// } while (g >= 0);

	// print "DONE YET AGAIN!";

	// // Increment and decrement operator tests
	// d = f--;

	// print "d after post decrement";
	// print d; // d = 5
	// print "f after post decrement";
	// print f; // f = 4

	// e = --f;

	// print "e after pre decrement";
	// print e; // e = 3
	// print "f after pre decrement";
	// print f; // f = 3

	// // Size of tests
	// print "SIZEOF c (int)";
	// d = sizeof(c); 
	// print d; // 4

	// print "SIZEOF h (array)";
	// d = sizeof(h);
	// print d; // 48

	return 0;
}