int glob[3];

void fun2(int a[]) {

  a[1] = 17;

}

int fun1(int a[]) {

  int c[3];

  c[1] = 0;
  fun2(c);
  print c[1];
  //printf("%d\n", c[1]);
  
  a[2] = 23;

  return 32;

  
}



int main (void) {

  int la[3];
  int y;

  y = fun1(glob);

  print glob[2];
  //printf("%d\n", glob[2]);  
  
  y = fun1(la);

  print la[2];
  //printf("%d\n", la[2]);  

  

}