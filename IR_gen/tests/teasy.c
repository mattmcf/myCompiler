int main(void) {
	int a = 10;

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