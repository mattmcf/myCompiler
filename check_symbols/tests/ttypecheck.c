int foo(int a, int b) {
	return a + b;
}

int bar(int a, int b) {
	int res;
	res = foo(a, b);
	return res;
}

int main(void) {
	int a;
	int b;
	int res;
	res = foo(a, b);

//	res = foo(z, b);
	
	res = bar(a, b);

//	hello();

	return 1;
}
