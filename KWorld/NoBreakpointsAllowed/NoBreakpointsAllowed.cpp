#include <Windows.h>
#include <stdio.h>

#include "ThreadHideFromDebugger.h"

int main() {
  printf("[+] Starting\n");

  if (ThreadHideFromDebugger::Enable()) {
    printf("[+] Thread hidden from debugger\n");
  } else {
    printf("[-] Problem hiding thread from debugger\n");
    return -1;
  }

  while (true) {
    printf(".");
    ::Sleep(250);
  }
}