int a[3];

int main(void){

  int i, n, b[3];

  for (i = 0; i < 3; i = i + 1) {
    a[i] = 0;
    b[i] = 0;
  }

  n = 1;

  ++a[++n];

  n = 1;

  ++b[++n];

  for (i = 0; i < 3; i = i + 1) {
    //    printf("a[%d] = %d, b[%d] = %d\n",i,a[i],i, b[i]);
    print a[i];
    print b[i];
  }

  return 0;
}