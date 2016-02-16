int array_f(int arr[],int i, int j, int z) {
	arr[10] = i;
	return arr[10];
}

int main(void) {
	int a[100], b;

	a[5] = 5;

	a[1] = array_f(a, a[0], b ,10);

	return 1;
} 

