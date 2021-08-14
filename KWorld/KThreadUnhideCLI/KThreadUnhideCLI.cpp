#include <Windows.h>

#include <iostream>

#include "..\Common\ku_shared.h"

int Error(const char* message) {
  auto code = GetLastError();

  printf("[!] %s (code %d)\n\n", message, code);

  LPSTR errorText = NULL;

  FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                 code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&errorText, 0, NULL);

  if (NULL != errorText) {
    printf("Message:\n%s\n", errorText);

    LocalFree(errorText);
    errorText = NULL;
  }

  return 1;
}

int Info(const char* message) {
  printf("[+] %s\n", message);
  return 0;
}

int main(char argc, char** argv) {
  HANDLE device = INVALID_HANDLE_VALUE;
  BOOL status = FALSE;
  DWORD bytesReturned = 0;

  // Data going to the driver
  ProcessData data;

  // Data coming back from the kernel
  ProcessDataComplete kData;

  if (argc < 2) {
    printf("Usage: %s <pid>\n", argv[0]);
    return 0;
  }

  // ProcessId to ask the driver to disable ThreadHideFromDebugger
  data.ProcessId = atoi(argv[1]);

  device = CreateFileW(L"\\\\.\\KmdWorld", GENERIC_ALL, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);

  if (device == INVALID_HANDLE_VALUE) {
    Error("Failed to open device");
    return 1;
  }

  printf("[+] Issuing IOCTL_THREAD_UNHIDE_FROM_DEBUGGER 0x%x for PID %lu\n", IOCTL_THREAD_UNHIDE_FROM_DEBUGGER,
         data.ProcessId);
  status = DeviceIoControl(device, IOCTL_THREAD_UNHIDE_FROM_DEBUGGER, &data, sizeof(data), &kData, sizeof(kData),
                           &bytesReturned, (LPOVERLAPPED)NULL);

  printf("[+] NumThreadsUnhidden: %d\n", kData.NumThreadsUnhidden);

  if (status) {
    Info("Success");
  } else {
    Error("DeviceIoControl error");
  }

  CloseHandle(device);
  return 0;
}