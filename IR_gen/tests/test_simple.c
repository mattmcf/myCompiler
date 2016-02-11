int foo(int a) {
  return 1;
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

  while (a == 7) {
  	a = 1;
  }

  for (b = 0; b < 10; ++b) {
  	print b;
  }

  return 0;
}