/*
 * stress testing with a sorting function
 */

//#include <stdio.h>


void sort(int arr[], int size) {
	int i, j;
	int lowest_seen, lowest_index;

	/* uncomment below line to see sizeof() error out for array handles */
	// sizeof(arr);

	for (i = 0; i < (size - 1); i++) {
		lowest_index = i;

		// search rest of array
		for (j = i + 1; j < size; j++) {

			// find lowest element
			if (arr[j] < arr[lowest_index]) {
				lowest_index = j;
			}

		}

		// swap in place
		if (lowest_index != i) {
			arr[i] = arr[i] - arr[lowest_index];
			arr[lowest_index] = arr[lowest_index] + arr[i];
			arr[i] = arr[lowest_index] - arr[i];				
		}
	}

	return;
}

int main(void) {
	int i;
	int arr[10];

	for (i = 0; i < 10; ++i) {
		// printf("Give me a number:\n");
		// scanf("%d",arr + i);

		print "Give me a number (signed hex):";
		read arr[i];
	}

	//printf("Starting sort routine\n");
	print "Starting sort routine";
	sort(arr, 10);

	//printf("Sorted Array: \n");
	print "Sorted array:";
	for (i = 0; i < 10; i++) {
		//printf("%d\n",arr[i]);
		print arr[i];
	}

	return 0;
}
