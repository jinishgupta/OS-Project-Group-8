#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: tryint <number>\n");
    exit(1);
  }
  int n = atoi(argv[1]);
  printf("tryint(%d) returned: %d\n", n, tryint(n));
  exit(0);
}
