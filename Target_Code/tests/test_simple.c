int a = 100;
int b = 200;

int foo(int a) {
  int c = 10;

  c = c + c * c / c % c || c && c;
  {
    a = c + a;
  }
  c = c + c * c / c % c || c && c;

  return 1;
}

void emptyfunc() {
  print "this is the empty function\n";
  return;
}

int main(void) {
  int a = 10;
  int b;

  a = 7;

  b = foo(a);

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