#include "UtilNt.h"

#include <ntddk.h>

//#include <winternl.h>
#include "..\Common\DriverCommon.h"
#include "undocumented.h"

NTSTATUS GetSystemProcessInfo(HANDLE pid, _Out_ PSYSTEM_PROCESS_INFORMATION SystemProcessInfo) {
  NTSTATUS status;
  PVOID buffer;
  PSYSTEM_PROCESS_INFORMATION spi;

  // Query needed length.
  ULONG length;

  /*
  https://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/query.htm?tx=76&ts=0,1285
  ... (Roughly, the expected size tends to be returned for
  information classes for which the output can vary in size, but tends not to be for information classes whose output is a
  fixed-size structure.)
  */
  status = _ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, NULL, 0, &length);

  if (status != STATUS_INFO_LENGTH_MISMATCH) {
    return STATUS_UNSUCCESSFUL;
  }

  /*
  https://github.com/tigros/HookTools/blob/master/traverse_threads.c
  retlen is a rough estimate, in bytes, of the amount of memory needed.
  the actual amount of memory needed depends on the number of threads
  and processes at exactly the time the function is called.
  double the size of the length to make sure there's enough space.
  */
  length *= 2;

  buffer = ExAllocatePool2(POOL_FLAG_PAGED, length, DRIVER_POOL_TAG);

  if (!buffer) {
    DbgPrintPrefix("Couldn't allocate memory for SystemInformation");
    return STATUS_UNSUCCESSFUL;
  }

  spi = (PSYSTEM_PROCESS_INFORMATION)buffer;

  status = _ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, spi, length, NULL);

  if (!NT_SUCCESS(status)) {
    ExFreePoolWithTag(spi, DRIVER_POOL_TAG);
    return STATUS_UNSUCCESSFUL;
  }

  BOOLEAN found = FALSE;

  while (spi->NextEntryOffset) {
    if (spi->UniqueProcessId == pid) {
      *SystemProcessInfo = *spi;
      found = TRUE;
      break;
    }

    // Next entry.
    spi = (PSYSTEM_PROCESS_INFORMATION)PtrAdd(spi, spi->NextEntryOffset);
  }

  ExFreePoolWithTag(buffer, DRIVER_POOL_TAG);

  if (!found)
    return STATUS_NOT_FOUND;

  return STATUS_SUCCESS;
}