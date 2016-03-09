int factorial(int arg) {

	if (arg > 0)
		return arg * factorial(arg - 1);
	else
		return 1;
}

int main() {
	
	int a, a_factorial;

	print "Give a positive input in hex (less than 0xd though!):";
	read a;

	if (a < 0) {
		print "Input was not positive!";
		return 1;

	} else if (a > 12) {
		print "Input was too large (overflow)!";
		return 1;

	} else {
		print "Calculating factorial for...";
		print a;
	}

	a_factorial = factorial(a);

	print "Result:";
	print a_factorial;

	return 0;
}