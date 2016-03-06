int global_int;
int global_arr[10];

int main() {
	int local_int;
	int local_arr[10];
	int a = 1, b = 2;
	int result;

	int ind1 = 0, ind2 = 9;

	local_arr[ind1] = 14;
	local_arr[ind2] = 15;
	global_arr[ind1] = 13;		
	global_arr[ind2] = 12;

	print local_arr[ind1];		// should be 0xe
	print local_arr[ind2];		// should be 0xf
	print global_arr[ind1];		// should be 0xd
	print global_arr[ind2];		// should be 0xc
	result = local_arr[9];
	local_arr[9] = 4;
	local_arr[0] = 2;

	local_arr[a + b] = 8;
	print local_arr[a + b]; 	// should be 0x8

	global_arr[a + b] = 4;
 	print global_arr[a + b]; 	// should be 0x4


	local_int 		= 14; 		// should be 0xe
	print local_int;

	global_int 		= 15; 		// should be 0xf
	print global_int;

	fill_arr(2, local_arr, 10); 
	fill_arr(8, local_arr, 11); 
	fill_arr(2, global_arr, 12); 
	fill_arr(8, global_arr, 13);  

	print local_arr[2]; 		// should be 0xa
	print local_arr[8]; 		// should be 0xb
	print global_arr[2]; 		// should be 0xc
	print global_arr[8]; 		// should be 0xd

	print fill_arr(5, local_arr, 5); 	// should be 0x5
	print fill_arr(5, global_arr, 6);	// should be 0x6
	//print result;
	return result;
}

int fill_arr(int i, int arr[], int val) {
	arr[i] = val; 			
	return arr[i];
}