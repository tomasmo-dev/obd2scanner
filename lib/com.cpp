#include "com.h"

ComPort* comPorts = NULL;
ComPort selectedPort;

// Function to list available serial devices
// check if the returned value is NULL, no devices found or error occurred
ComPort* listDevices() {
	#if defined(_WIN32)
	HKEY hKey;
	LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &hKey);
	if (result != ERROR_SUCCESS) {
		printf("Failed to open registry key. Error code: %ld\n", result);
		return NULL;
	}

	char valueName[256];
	char data[256];
	DWORD type;
	DWORD index = 0;

	while (1) {
		DWORD valueNameSize = sizeof(valueName);
		DWORD dataSize = sizeof(data);

		result = RegEnumValueA(hKey, index, valueName, &valueNameSize, NULL, &type, (LPBYTE)data, &dataSize);
		if (result != ERROR_SUCCESS) break;

		// allocate memory for the port name and copy it
		char* portName = (char*)malloc(dataSize + 1);
		strcpy(portName, data);

		ComPort port;
		port.portName = portName;
		ds_push(comPorts, port);
	}

	RegCloseKey(hKey);

	return comPorts;

	#elif defined(ESP_PLATFORM) || defined(ESP32)


	return comPorts;
	#endif
}