/*
 * testing out various ways to declare and call functions
 */

void f2(void){
	int b;
}

int f3(int arg1, int arg2){
	int c;

	return arg1 + arg2;	
}

int main(void){
	int a;
	int b;
	int c;
	int d;
	{
		int z;
	}

	a = f3(b, c);

	f2();

	return 0;
}
