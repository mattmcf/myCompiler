int a = 10;
int arr[500];

int foo(int a, int b) {
	return a + b;
}

int bar(int a, int b) {
	int res;
	res = foo(a, b);
	return res;
}

int main(void) {
	int a;
	int b;
	int res;
	res = foo(a, b);

//	res = foo(z, b);
	
	res = bar(a, b);

//	emptyfunc(arr[501],a[5],arr);
	emptyfunc(arr[501],a,arr);

	res = get_val(arr,arr[0]);

//	hello();

	return 1;
}

void emptyfunc(int a, int b, int c[]) {
	for (a = 0; a > b; ++a) {
		--c[a];
		print (b * c[a]);
	}
}

void ef() {
}

int get_val(int arr[], int val) {
	return arr[val];
}
