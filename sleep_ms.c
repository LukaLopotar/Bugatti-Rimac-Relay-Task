#include "sleep_ms.h"

#include <stdint.h>

#ifdef _WIN32
#include <windows.h>

int8_t sleep_ms(unsigned int time_ms) {
  Sleep(time_ms);
  return 0;
}
#else // Assume Linux platform
#include <time.h>

int8_t sleep_ms(unsigned int time_ms) {
  struct timespec req = {0};
  req.tv_sec = time_ms / 1000;
  req.tv_nsec = (time_ms % 1000) * 1000000L;

  nanosleep(&req, NULL);

  return 0;
}

#endif
