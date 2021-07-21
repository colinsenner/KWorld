#include <ntddk.h>
#include <wdf.h>

#include "DrvCommon.h"

DRIVER_INITIALIZE DriverEntry;

VOID KmdWorldUnload(IN WDFDRIVER Driver) {
  UNREFERENCED_PARAMETER(Driver);

  DbgPrintPrefix("DriverUnload");
}

NTSTATUS
DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
  // NTSTATUS variable to record success or failure
  NTSTATUS status = STATUS_SUCCESS;
  WDFDRIVER driver;
  WDF_DRIVER_CONFIG config;

  // Initialize the driver configuration object to register the
  // entry point for the EvtDeviceAdd callback, KmdfHelloWorldEvtDeviceAdd
  WDF_DRIVER_CONFIG_INIT(&config, NULL);

  config.DriverInitFlags = WdfDriverInitNonPnpDriver;
  config.EvtDriverUnload = KmdWorldUnload;

  // Finally, create the driver object
  status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, &driver);

  DbgPrintPrefix("DriverEntry (0x%p, %wZ)", DriverObject, RegistryPath);

  return status;
}
