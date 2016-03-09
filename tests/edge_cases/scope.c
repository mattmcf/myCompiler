int x;

void sub(int x) {

  x = 3;
  print x;

}

int main(void) {

  x = 1;

  // printf("%d\n", x);
  print x;

  {
    int x;

    x = 2; 
    sub(x);

    print x;
  }

  print x;

  return 0;

}