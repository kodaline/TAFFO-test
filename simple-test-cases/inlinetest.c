///TAFFO_TEST_ARGS 
#include <stdio.h>


float hello(__attribute__((annotate("range -200 200"))) float abc) __attribute__((always_inline)) {
  return abc + (float)5.0;
}


int main(int argc, char *argv[]) {
	__attribute__((annotate("range -200 200"))) float test = 123.0;
	test = hello(test);
	printf("%f\n", test);
	return 0;
}

