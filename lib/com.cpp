#if defined(_WIN32)
	#include <windows.h>
	#include <stdio.h>
	HANDLE hCom;

#elif defined(ESP_PLATFORM) || defined(ESP32)
	#include "BluetoothSerial.h"
	BluetoothSerial SerialBT;

#else
	#error "Unsupported platform (WIN or ESP)"
#endif


