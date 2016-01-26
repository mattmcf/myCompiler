int main(void) {
  int a = 10;

  if (a == 10) {
    a = 2;

    while (a > 3) {
      a = 3;
    }

    while (a > 7) {
      a = 7;
    }
  }

  if (a == 7) {
    a = 4;

    while (a > 2) {
      a = 2;
    }

    while (a > 9) {
      a = 9;
    }
  }
}