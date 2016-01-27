int a;
int b = 10;
int c, d, e, f;
int arr[100];

int myfunc1(int arg1, int arg2){
	int local_var1;
	int local_arr1[100];

	return 0;
}

int main(void) {
	int main_var;

	{
		int inner_var1, inner_var2;
		int inner_var3;
	}

	if (main_var == 10)
		main_var = -10;
	else
		main_var = 0;

	for (main_var = 0; main_var < 100; ++main_var) {
		int for_var;
		print for_var;
	}
	return 0;
}
