int array_f(int arr[],int i) {
	arr[10] = i;
	return arr[10];
}

int main(void) {
	int a[100];

	a[5] = 5;

	a[1] = array_f(a,5);

	return 1;
} 

