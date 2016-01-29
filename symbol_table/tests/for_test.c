int main(){
	int i;
	for (i = 0; i < 10; ++i) {
		print i;
	}

	for (;;){
		;
	}

	for (;;)
		;

	while (1)
		;

	while (1)  {;}

	// below invocation doesn't work
	// do 
	// while (1);

	{}

	{;}

	myfunc();

	return 0;
}

int emptyFunc() {
	return 1;
}

int fullFunc(int a, int b, int c[]) {
	return 0;
}