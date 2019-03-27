///TAFFO_TEST_ARGS -Xinit -mem2reg
#include <stdio.h>

float test(float a)
{
  return a*2;
}

int main(int argc, char *argv[])
{
  float a __attribute((annotate("target('a') scalar(range(-128, 128) disabled)")));
  scanf("%f", &a);
  float b __attribute((annotate("target('a') scalar(range(-256, 256))"))) = a*2;
  printf("%f\n", test(b));
  return 0;
}

