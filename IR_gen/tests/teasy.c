int foo(int a, int b) {
	{
		int c = a + b;
	}

	return a + b;
} 

int main(void) {
	int a = 10;
	int b;
	int c;
	
	if (a == 10) {
		a = -a;
	} else {
		a = a + 10;
	}

	while (a != 12) {
		print a;
	}

	for (a = 3; a < 21; ++a) {
		read a;
	}

	if (a == 17) {
		a = 2;
	} else {
		a = 1;
	}

	b = 1 + 8;

	a = b + 7;


	a = 1 + 7;

	while (a == 10) {
		a = 7;
	}
	
	a = !(a == 1);
	print "hello";
	
	a = foo(b, c);

	while (a == 10) {
		int a = 6;
		int b = 10;
		a = 7;
		{
			int c = 0;
			int d = 1;
		}
	}

	a = 7;

	a = 7;

	a = 1 + 7;

	a = 1 - 7;

	a = 1 * 7;

	a = 1 / 7;

	++a;
	
	--a;

	return 0;
}

int new_f(int a1, int a2) {
	int x,y,z = 10;

	{
		int m = z;
	}

	return 0;
}