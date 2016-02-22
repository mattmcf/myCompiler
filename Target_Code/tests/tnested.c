int main(void) {
	int a = 1;;

	print a; // 1

	{
		int a = 2;

		print a; // 2

		{
			int a = 3;

			print a; // 3
		}

		print a; // 2
	}

	print a; // 1

	return 0;
}