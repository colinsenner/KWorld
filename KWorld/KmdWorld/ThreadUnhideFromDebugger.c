#include "ThreadUnhideFromDebugger.h"

#include <ntddk.h>

#include "..\Common\DriverCommon.h"
#include "undocumented.h"

NTSTATUS UnhideThread(HANDLE tid) {
  UNREFERENCED_PARAMETER(tid);

  return STATUS_SUCCESS;
}

NTSTATUS ThreadUnhideFromDebugger(HANDLE pid) {
  NTSTATUS status = STATUS_SUCCESS;
  PSYSTEM_PROCESS_INFORMATION spi;

  // Query needed length.
  ULONG ReturnLength;
  PVOID Buffer;

  BOOLEAN found = FALSE;

  /*
  https://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/query.htm?tx=76&ts=0,1285
  ... (Roughly, the expected size tends to be returned for
  information classes for which the output can vary in size, but tends not to be for information classes whose output is
  a fixed-size structure.)
  */
  status = _ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, NULL, 0, &ReturnLength);

  if (!NT_SUCCESS(status) && status != STATUS_INFO_LENGTH_MISMATCH) {
    return status;
  }

  /*
  https://github.com/tigros/HookTools/blob/master/traverse_threads.c
  retlen is a rough estimate, in bytes, of the amount of memory needed.
  the actual amount of memory needed depends on the number of threads
  and processes at exactly the time the function is called.
  double the size of the length to make sure there's enough space.
  */
  ReturnLength *= 2;

  Buffer = ExAllocatePool2(POOL_FLAG_PAGED, ReturnLength, DRIVER_POOL_TAG);
  spi = (PSYSTEM_PROCESS_INFORMATION)Buffer;

  if (!Buffer) {
    DbgPrintPrefix("[!] Couldn't allocate memory for SystemProcessInformation");
    return STATUS_MEMORY_NOT_ALLOCATED;
  }

  status = _ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, spi, ReturnLength, NULL);

  if (!NT_SUCCESS(status)) {
    ExFreePoolWithTag(Buffer, DRIVER_POOL_TAG);
    DbgPrintPrefix("[!] ZwQuerySystemInformation failed");
    return status;
  }

  // Go through each process running
  while (spi->NextEntryOffset) {
    if (spi->UniqueProcessId == pid) {
      DbgPrintPrefix("[+] Found process: %llu", (ULONG_PTR)pid);

      // Enumerate all threads of this process
      for (ULONG thread_index = 0; thread_index < spi->NumberOfThreads; ++thread_index) {
        PSYSTEM_THREAD_INFORMATION ti = &spi->Threads[thread_index];

        if (ti->ClientId.UniqueProcess != spi->UniqueProcessId)
          continue;

        DbgPrintPrefix("  tid: %llu", (ULONG_PTR)ti->ClientId.UniqueThread);
        UnhideThread(ti->ClientId.UniqueThread);
      }

      found = TRUE;
      break;
    }

    // Next process
    spi = (PSYSTEM_PROCESS_INFORMATION)PtrAdd(spi, spi->NextEntryOffset);
  }

  if (!found)
    status = STATUS_NOT_FOUND;

  ExFreePoolWithTag(Buffer, DRIVER_POOL_TAG);

  return status;
}