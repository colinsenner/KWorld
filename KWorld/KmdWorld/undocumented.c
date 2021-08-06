#include "undocumented.h"

#include <wdm.h>

// Function pointers
fpZwQuerySystemInformation _ZwQuerySystemInformation = NULL;

NTSTATUS InitUndocumented() {
  if (!_ZwQuerySystemInformation) {
    UNICODE_STRING routineName;
    RtlInitUnicodeString(&routineName, L"ZwQuerySystemInformation");
    _ZwQuerySystemInformation = (fpZwQuerySystemInformation)MmGetSystemRoutineAddress(&routineName);

    if (!_ZwQuerySystemInformation)
      return STATUS_UNSUCCESSFUL;
  }

  return STATUS_SUCCESS;
}
