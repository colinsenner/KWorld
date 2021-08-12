#include "undocumented.h"

#include <wdm.h>

namespace nt_undocumented {

// Function pointers
fpZwQuerySystemInformation _ZwQuerySystemInformation = NULL;

NTSTATUS Init() {
  if (!_ZwQuerySystemInformation) {
    UNICODE_STRING routineName;
    RtlInitUnicodeString(&routineName, L"ZwQuerySystemInformation");
    _ZwQuerySystemInformation = (fpZwQuerySystemInformation)MmGetSystemRoutineAddress(&routineName);

    if (!_ZwQuerySystemInformation)
      return STATUS_UNSUCCESSFUL;
  }

  return STATUS_SUCCESS;
}
}  // namespace nt_undocumented