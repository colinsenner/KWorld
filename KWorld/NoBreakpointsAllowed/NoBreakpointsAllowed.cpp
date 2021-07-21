#include <Windows.h>
#include <stdio.h>

#include "ThreadHideFromDebugger.h"

int main() {
  printf("[+] Starting\n");
  printf("[+] Main thread id: 0x%X\n", GetCurrentThreadId());

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