#include <windows.h>
#include <stdio.h>
#include <string.h>

int main() {
  HANDLE hComm;
    
  // 1. Open the virtual Bluetooth COM port
  // Note: The "\\\\.\\" prefix is required for ports COM10 and above, 
  // but it's good practice to use it for all COM ports.
  hComm = CreateFile("\\\\.\\COM4",                // Port name (change to match your paired Vgate)
                     GENERIC_READ | GENERIC_WRITE, // Read/Write
                     0,                            // No sharing
                     NULL,                         // No security
                     OPEN_EXISTING,                // Open existing port only
                     0,                            // Non overlapped I/O
                     NULL);                        // Null for comm devices

  if (hComm == INVALID_HANDLE_VALUE) {
    printf("Error: Could not open port. Check if it's paired and the COM number is correct.\n");
    return 1;
  }

  // 2. Configure the serial port parameters (Baud rate, 8N1)
  DCB dcbSerialParams = {0};
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    
  if (GetCommState(hComm, &dcbSerialParams)) {
    dcbSerialParams.BaudRate = CBR_38400;   // 38400 baud
    dcbSerialParams.ByteSize = 8;           // 8 data bits
    dcbSerialParams.StopBits = ONESTOPBIT;  // 1 stop bit
    dcbSerialParams.Parity   = NOPARITY;    // No parity
    SetCommState(hComm, &dcbSerialParams);
  }

  // 3. Set read timeouts so ReadFile doesn't hang waiting for data
  COMMTIMEOUTS timeouts = {0};
  timeouts.ReadIntervalTimeout         = 50;
  timeouts.ReadTotalTimeoutConstant    = 500; // Wait 500ms for response
  timeouts.ReadTotalTimeoutMultiplier  = 10;
  SetCommTimeouts(hComm, &timeouts);

  // 4. Send the reset command
  char *cmd = "AT Z\r";
  DWORD bytesWritten;
    
  if (!WriteFile(hComm, cmd, strlen(cmd), &bytesWritten, NULL)) {
    printf("Error: Failed to write to port.\n");
    CloseHandle(hComm);
    return 1;
  }

  // 5. Read the response
  char buffer[256];
  memset(buffer, 0, sizeof(buffer));
  DWORD bytesRead;

  // ReadFile will wait up to the timeout specified above
  if (ReadFile(hComm, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
    if (bytesRead > 0) {
      printf("Success! Adapter says: %s\n", buffer);
    } else {
      printf("Read timeout: No response from adapter.\n");
    }
  } else {
    printf("Error: Failed to read from port.\n");
  }

  // 6. Cleanup
  CloseHandle(hComm);
  return 0;
}
