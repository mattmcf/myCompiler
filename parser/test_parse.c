int global_var1, g_var2, g_var3[9999];

int main(void) {
	// all variable declarations must be before statements
	int a, b = 10;
	int ans;
	int i;

	a = 25;

	ans = myFunction(a, b);

	print "hello world!\n";

	for (i = 0; i < 15; ++i) {
		print (i);
	}

	i = 100;
	do {
		--i;
	} while (i > 0);

	if (ans != 5) {
		ans = 5 * i / b % 101 - 6 + 18;
	} else {
		ans = 10;
	}

	if (ans == 10) {
		ans = 15;
	} 

	for ( i = 17; i <= 20; i = i * 2) {
		print (i >= 5);
	}

	while ( (i != 5) && (a == 10) ) {
		print "i should be 5 and a needs to be other than 10\n";
		print "i : ";
		read i;
		print "a : ";
		read a;
	}

	return 0;
}

int myFunction( int arg1, int arg2 ) {
	int my_var, my_var2;
	return (arg1 + arg2) * my_var;
}