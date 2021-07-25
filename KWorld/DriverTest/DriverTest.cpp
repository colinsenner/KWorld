#include <Windows.h>

#include <iostream>

#include "..\Common\DriverCommon.h"

int main(char argc, char** argv) {
  HANDLE device = INVALID_HANDLE_VALUE;
  BOOL status = FALSE;
  DWORD bytesReturned = 0;
  CHAR outBuffer[128] = {0};

  if (argc < 2) {
    printf("Usage: %s <pid>\n", argv[0]);
    return 0;
  }

  // PID to ask the driver to disable ThreadHideFromDebugger
  size_t pid = atoi(argv[1]);

  device = CreateFileW(L"\\\\.\\KmdWorld", GENERIC_ALL, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);

  if (device == INVALID_HANDLE_VALUE) {
    printf("[-] Could not open device: 0x%x\n", GetLastError());
    return 0;
  }

  printf("[+] Issuing IOCTL_THREAD_UNHIDE_FROM_DEBUGGER 0x%x for PID %zu\n", IOCTL_THREAD_UNHIDE_FROM_DEBUGGER, pid);
  status = DeviceIoControl(device, IOCTL_THREAD_UNHIDE_FROM_DEBUGGER, &pid, sizeof(pid), outBuffer, sizeof(outBuffer),
                           &bytesReturned, (LPOVERLAPPED)NULL);

  if (status) {
    printf("[+] Success\n");
  } else {
    printf("[-] Error: %d\n", GetLastError());
  }

  CloseHandle(device);
  return 0;
}