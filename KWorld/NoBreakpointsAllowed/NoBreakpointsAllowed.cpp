#include <Windows.h>
#include <stdio.h>

#include "ThreadHideFromDebugger.h"

int main() {
  printf("[+] Starting\n");
  printf("[+] PID: %d (0x%X)\n", GetCurrentProcessId(), GetCurrentProcessId());
  printf("[+] Main thread id: %d (0x%X)\n", GetCurrentThreadId(), GetCurrentThreadId());

  if (ThreadHideFromDebugger::Enable()) {
    printf("[+] Thread hidden from debugger\n");
  } else {
    printf("[-] Problem hiding thread from debugger\n");
    return -1;
  }

  while (true) {
    printf(".");
    ::Sleep(1000);
  }

  printf("[+] Exit\n");
}