#include "UtilNt.h"

#include <ntddk.h>

//#include <winternl.h>
#include "..\Common\DriverCommon.h"
#include "undocumented.h"

NTSTATUS GetSystemProcessInfo(HANDLE pid, _Out_ PSYSTEM_PROCESS_INFORMATION SystemProcessInfo) {
  NTSTATUS status;
  PVOID buf;
  PSYSTEM_PROCESS_INFORMATION spi;

  // Query needed length.
  ULONG length;
  status = _ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, NULL, 0, &length);

  if (status != STATUS_INFO_LENGTH_MISMATCH) {
    return STATUS_UNSUCCESSFUL;
  }

  buf = ExAllocatePool2(POOL_FLAG_PAGED, length, DRIVER_POOL_TAG);

  if (!buf) {
    DbgPrintPrefix("Couldn't allocate memory for SystemInformation");
    return STATUS_UNSUCCESSFUL;
  }

  spi = (PSYSTEM_PROCESS_INFORMATION)buf;

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

  ExFreePoolWithTag(buf, DRIVER_POOL_TAG);

  if (!found)
    return STATUS_NOT_FOUND;

  return STATUS_SUCCESS;
}