int main() {
	int a[10];
	//int *b = &a[9]; 	// more rigorous test for gcc also fails
	int b[10];

	a = b;

	return 0;
}
