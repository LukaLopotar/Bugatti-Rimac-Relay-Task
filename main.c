#include <stdbool.h>
#include <stdio.h>
#include "sleep_ms.h"

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  // Scheduler mock:
  while (true) {
    printf("Sleeping for 10 ms.\n");
    sleep_ms(10);
  }

  return 0;
}