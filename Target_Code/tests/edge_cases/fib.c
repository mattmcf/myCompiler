//#include <stdio.h>

int x;

int fib(int a) {
  if(a <= 1)
    return 1;

	//  print "fibbing";
	//  print (a-1);
	//  print (a-2);
  return fib(a-1) + fib(a-2);
}
  
int main() {
  int i;

  for (i = 1; i <=7; i=i+1) {
    // printf("%d\n", fib(i));
    // print "i is";
    // print i;
    print fib(i);
  }
  return 0;
}
