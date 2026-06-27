int pointerAlias() {
  int x = 5;
  int *p = &x;

  *p = 10;

  return x;
}