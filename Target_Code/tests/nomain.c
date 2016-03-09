/*
 * should throw an error during compilation because no main is declared
 */

int my_func(int arg1) {
	int a, b, c;

	return a + b + c + arg1;
}