int a, b, c;

int foo(int a) {
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

  while (a == 7) {
  	a = 1;
  }

  for (b = 0; b < 10; ++b) {
  	print b;
    emptyfunc();
  }

  return 0;
}