#include <stdio.h>

#include "com.h"

int main() {
  ComPort* devices = listDevices();

  printf("Number of devices found: %d\n", DS_COUNT(devices));
  return 0;
}
