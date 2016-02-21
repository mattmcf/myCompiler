int global_int;
int global_arr[10];

int main() {
	int local_arr[10];

	// global_int = 25;
	// global_arr[0] = 5;
	// global_arr[9] = 6;
	// local_arr[0] = 7;
	// local_arr[9] = 8;

	fill_arr(0, global_arr);
	fill_arr(9, global_arr);

	

	return 0;
}

int fill_arr(int i, int arr[]) {
	arr[i] = 42;
	return 1;
}