#include <ntddk.h>
#include <wdf.h>

#include "DrvCommon.h"

DRIVER_INITIALIZE DriverEntry;

// Globals
PDRIVER_OBJECT g_DriverObject;

void NTAPI DriverUnload(PDRIVER_OBJECT DriverObject) {
  UNREFERENCED_PARAMETER(DriverObject);

  DbgPrintPrefix("DriverUnload");
}

NTSTATUS
DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
  // NTSTATUS variable to record success or failure
  NTSTATUS status = STATUS_SUCCESS;

  // Print "Hello World" for DriverEntry
  DbgPrintPrefix("DriverEntry (0x%p, %wZ)", DriverObject, RegistryPath);

  g_DriverObject = DriverObject;

  DriverObject->DriverUnload = DriverUnload;

  return status;
}
