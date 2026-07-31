#include "com.h"

ComPort* comPorts = NULL;
ComPort selectedPort;

#if defined(_WIN32)
HANDLE hCom = NULL;

#elif defined(ESP_PLATFORM) || defined(ESP32)
BluetoothSerial SerialBT;

#endif

// Function to list available serial devices
// check if the returned value is NULL, no devices found or error occurred
ComPort* listDevices() {
	#if defined(_WIN32)
	HKEY hKey;
	LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &hKey);
	if (result == ERROR_FILE_NOT_FOUND) {
		printf("No serial devices found.\n");
		return NULL;
	} else if (result != ERROR_SUCCESS) {
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
		if (result == ERROR_NO_MORE_ITEMS) break;
		else if (result != ERROR_SUCCESS) break;

		// allocate memory for the port name and copy it
		char* portName = (char*)malloc(dataSize + 1);
		strcpy(portName, data);

		ComPort port;
		port.portName = portName;
		ds_push(comPorts, port);

		index++;
	}

	RegCloseKey(hKey);

	return comPorts;

	#elif defined(ESP_PLATFORM) || defined(ESP32)


	return comPorts;
	#endif
}

#if defined(_WIN32)
HANDLE openComPort(ComPort* port, int baudrate) {
	// port->portName;

	char portName[20]; // maximum com port name shoud be 16 characters when using extended namespace syntax
	sprintf(portName, "\\\\.\\%s", port->portName);

	hCom = CreateFileA(portName,
					GENERIC_READ | GENERIC_WRITE,
					0,
					NULL,
					OPEN_EXISTING,
					0,
					NULL);

	DCB dcb_param = {0};
	dcb_param.DCBlength = sizeof(dcb_param);

	GetCommState(hCom, &dcb_param);
	
	if (baudrate = 0) {
		dcb_param.BaudRate = 9600;       // other rates 4800,19200,38400etc 

	} else {
		dcb_param.BaudRate = baudrate;
	}
    dcb_param.ByteSize = 8;          // data word length = 8 bits per byte.Common values are 5, 6, 7, or 8
    dcb_param.Parity = NOPARITY;     // NOPARITY, ODDPARITY, EVENPARITY, MARKPARITY, or SPACEPARITY
    dcb_param.StopBits = ONESTOPBIT; // ONESTOPBIT, ONE5STOPBITS, or TWOSTOPBITS

	SetCommState(hCom, &dcb_param);

	PurgeComm(hCom, PURGE_RXCLEAR | PURGE_TXCLEAR); // purge the connection before using it

	return hCom;
}
void closeComPort() {
	CloseHandle(hCom);
}

void sendCommand(const char* command) {
	DWORD written = 0;

	WriteFile(hCom, command, strlen(command), &written, NULL);
}

char* readConnection() {

	char* output = NULL;
	COMMTIMEOUTS timeouts = {0};

	timeouts.ReadIntervalTimeout = 20; // max time in between bytes
	timeouts.ReadTotalTimeoutMultiplier = 1;  // Per-byte timeout
    timeouts.ReadTotalTimeoutConstant = 50; // Constant timeout (ms)

    timeouts.WriteTotalTimeoutMultiplier = 1; // Per-byte timeout
    timeouts.WriteTotalTimeoutConstant = 50;// Constant timeout (ms)

	SetCommTimeouts(hCom, &timeouts);

	DWORD bytes_read = 0;
	bool reading = true;

	char buffer[256] = {0};

	while (reading) {
		ReadFile(hCom,
				buffer,
				sizeof(buffer),
				&bytes_read,
				NULL);

		if (bytes_read < 256) {
			reading = false;
		}

		for (int i = 0; i < bytes_read; i++) {
			ds_push(output, buffer[i]);
		}
	}

	return output;
}

#elif defined(ESP_PLATFORM) || defined(ESP32)

#endif