int max = 100;
int arr[100];

int fill_arr(int i, int j) {
	int rc;
	if (i == j) {
		int z;
		rc = j;
	} else {
		rc = -j;
	}

	return rc;
}

int main(void) {
	int i, j;

	/* test nested loops */
	for (i = 0; i < max; ++i) {
	 	int for1;
	 	++for1;
	 	for (j = 0; j < max; ++j ){
	 		int for2;
	 		--for2;
	 		arr[i] = fill_arr(i,j);
	 	}
	}

	return 0;
}

void arr_arg(int to_fill[], int size) {
	int i;
	for (i = 0; i < size; ++i) {
		to_fill[i] = i * i;
	}
	return;
}