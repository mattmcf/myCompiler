int global_int;
int global_arr[10];

int main() {
	int local_int;
	int local_arr[10];
	int a = 1, b = 2;
	int result;

	local_arr[a + b] = 8;
	local_arr[a] = 4;
	local_arr[0] = 2;

	print local_arr[a + b]; 	// should be 0x4
	print local_arr[a]; 		// should be 0x3
	print local_arr[0]; 		// should be 0x2

	local_int 		= 14; 		// should be 0xe
	print local_int;

	global_int 		= 15; 		// should be 0xf
	print global_int;

	fill_arr(0, local_arr, 10); 
	fill_arr(9, local_arr, 11); 
	fill_arr(0, global_arr, 12); 
	fill_arr(9, global_arr, 13);  

	print local_arr[0]; 		// should be 0xa
	print local_arr[9]; 		// should be 0xb
	print global_arr[0]; 		// should be 0xc
	print global_arr[9]; 		// should be 0xd

	return 0;
}

int fill_arr(int i, int arr[], int val) {
	arr[i] = val; 			
	return arr[i];
}