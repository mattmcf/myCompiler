/*
 * this test shows that you cannot assign array handles 
 * from one array to another
 */

int main() {
	int a[10];
	//int *b = &a[9]; 	// more rigorous test for gcc also fails
	int b[10];

	a = b;

	return 0;
}
