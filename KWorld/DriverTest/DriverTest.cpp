#include <Windows.h>

#include <iostream>

#include "..\Common\DriverCommon.h"

int main(char argc, char** argv) {
  HANDLE device = INVALID_HANDLE_VALUE;
  BOOL status = FALSE;
  DWORD bytesReturned = 0;
  CHAR inBuffer[128] = {0};
  CHAR outBuffer[128] = {0};

  char message[] = "hello from userland";

  RtlCopyMemory(inBuffer, message, strlen(message));

  device = CreateFileW(L"\\\\.\\KmdWorld", GENERIC_ALL, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);

  if (device == INVALID_HANDLE_VALUE) {
    printf_s("> Could not open device: 0x%x\n", GetLastError());
    return FALSE;
  }

  printf_s("> Issuing IOCTL_THREAD_UNHIDE_FROM_DEBUGGER 0x%x\n", IOCTL_THREAD_UNHIDE_FROM_DEBUGGER);
  status = DeviceIoControl(device, IOCTL_THREAD_UNHIDE_FROM_DEBUGGER, inBuffer, sizeof(inBuffer), outBuffer,
                           sizeof(outBuffer), &bytesReturned, (LPOVERLAPPED)NULL);
  printf_s("> IOCTL_SPOTLESS 0x%x issued\n", IOCTL_THREAD_UNHIDE_FROM_DEBUGGER);
  printf_s("> Received from the kernel land: %s. Received buffer size: %d\n", outBuffer, bytesReturned);

  CloseHandle(device);

  system("Pause");
}