/*
 * shows scope tests
 */

int b = 10;


int main(void) {
	int a = 1;

	print a; // 1

	{
		int a = 2;

		print a; // 2
		print b; // 10

		{
			int b = 7;
			int a = 3;

			print a; // 3
			print b; // 7
		}

		print a; // 2
	}

	print a; // 1
	print b; // 10

	return 0;
}