/*
 * stress test the compiler with expressions
 */

int a = 100;
int b = 200;
int global_arr[100];
int x,y,z;

int foo(int a, int b, int z, int arr[]) {
  int c = 10;
  int foo_arr[100];
  int d;

  // generate a lot of temps
  c = c + c * c / c % c || c && c;
  {
    a = c + a;
  }
  // generate more temps!
  c = c + c * c / c % c || c && c + c + c * c / c % c || c && c + c + c * c / c % c || c && c + c + c * c / c % c || c && c;

  return 1;
}

void emptyfunc() {
  print "this is the empty function\n";
  return;
}

int main(void) {
  int a = 10;
  int b;
  int arr[100];

  a = 7;

  //b = foo(a);

  if (a == 7 || b == 1) {
  	print 7;
  } else {
  	print 0;
  }

  {
    a = a + 1;
    {
      int a = 10;
      {
        a = a + 10;
        {
          a = a + 10;
        }
      }
    }
  }

  while (a == 7) {
  	a = 1;
  }

  for (b = 0; b < 10; ++b) {
  	print b;
    emptyfunc();
  }

  return 0;
}