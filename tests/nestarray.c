/*
 * illustrates how the compiler can handle nested array index calcuations
 */

int main() {
	int arr1[10], arr2[10];

	int a = 5;
	arr1[a] = 4;
	arr1[4] = 3;
	arr2[3] = 9;
    arr1[arr2[arr1[arr1[a]]]] = 42;
    print arr1[arr2[arr1[arr1[a]]]];	// should be 0x2A
    print arr1[9];						// should also be 0x2A

	return 0;
}	
