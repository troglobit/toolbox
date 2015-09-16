#include <stdio.h>
#include <dlfcn.h>

extern int api(int num, char *str);

int main(void)
{
  printf("Calling api(42, 'Hello')\n");
  printf("\nReturns: %d\n", api(42, "Hello"));

  return 0;
}
