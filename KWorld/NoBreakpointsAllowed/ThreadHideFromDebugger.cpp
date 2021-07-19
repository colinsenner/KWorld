#include "ThreadHideFromDebugger.h"

#include <Windows.h>

bool ThreadHideFromDebugger::Enable() {
  typedef NTSTATUS(NTAPI * fp_NtSetInformationThread)(
      HANDLE ThreadHandle, THREAD_INFORMATION_CLASS ThreadInformationClass,
      PVOID ThreadInformation, ULONG ThreadInformationLength);

  HMODULE hNtdll = ::GetModuleHandle(L"ntdll.dll");

  if (!hNtdll)
    return false;

  auto addr = ::GetProcAddress(hNtdll, "NtSetInformationThread");

  if (!addr)
    return false;

  fp_NtSetInformationThread _NtSetInformationThread =
      (fp_NtSetInformationThread)addr;

  _NtSetInformationThread(GetCurrentThread(), (THREAD_INFORMATION_CLASS)0x11,
                          nullptr, 0);

  return true;
}
