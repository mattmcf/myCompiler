int main(void) {
	int e;
	int d;
	int f = 5;
	int a = 0;

	print "Should break out of loop immediately";

	while (a < 10) {
		int hello = 2;

		if (hello == 2) {
			break;
		}
	}

	print "Broke out of loop!";

	print "Should print inner 0, inner 1, outer 0, inner 0, inner 1, outer 1";

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

	print "Should print 5 to 0 skipping 4";
	
	do {
		if (f == 4) {
			f--;
			continue;
		}

		print f;
		f--;
	} while (f >= 0);

	return 0;
}