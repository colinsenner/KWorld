#include <Windows.h>
#include <stdio.h>

#include <thread>

#include "thread_hide_from_debugger.hpp"

void thread_two() {
  printf("[+] New thread created: %d (0x%X)\n", GetCurrentThreadId(), GetCurrentThreadId());

  while (true) {
    Sleep(1000);
  }
}

int main() {
  printf("[+] Starting\n");
  printf("[+] PID: %d (0x%X)\n", GetCurrentProcessId(), GetCurrentProcessId());
  printf("[+] Main thread id: %d (0x%X)\n", GetCurrentThreadId(), GetCurrentThreadId());

  std::thread th2(thread_two);

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