#include <stdio.h>

#include "com.h"

int main() {
  ComPort* devices = listDevices();

  if (devices == NULL) {
    printf("listDevices failed.\n");
    return 1;
  }

  printf("Number of devices found: %d\n", DS_COUNT(devices));

  printf("Opening: %s", devices->portName);
  HANDLE h = openComPort(devices);
  closeComPort(&h);
  return 0;
}
