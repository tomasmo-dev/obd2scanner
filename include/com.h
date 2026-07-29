#ifndef COM_H
#define COM_H

#include "ds_array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
	#include <windows.h>
	extern HANDLE hCom;

#elif defined(ESP_PLATFORM) || defined(ESP32)
	#include "BluetoothSerial.h"
	extern BluetoothSerial SerialBT;

#else
	#error "Unsupported platform (WIN or ESP)"
#endif

struct ComPort {
	#if defined(_WIN32)
		char* portName;
	#elif defined(ESP_PLATFORM) || defined(ESP32)
		char* deviceName;
		uint8_t macAddress[6];
	#endif
};

// Function to list available serial devices
// check if the returned value is NULL, no devices found or error occurred
ComPort* listDevices();

#if defined(_WIN32)
HANDLE openComPort(ComPort* port);
void sendCommand(HANDLE hCom, const char* command);
#elif defined(ESP_PLATFORM) || defined(ESP32)
BluetoothSerial openComPort(ComPort* port, const char* deviceName); // device name for ESP32
void sendCommand(BluetoothSerial& serial, const char* command);
#endif

#endif // COM_H