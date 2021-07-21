#include <ntddk.h>
#include <wdf.h>

#include "DrvCommon.h"

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD KmdfHelloWorldEvtDeviceAdd;

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

  // Allocate the driver configuration object
  WDF_DRIVER_CONFIG config;

  // Print "Hello World" for DriverEntry
  DbgPrintPrefix("DriverEntry");

  // Initialize the driver configuration object to register the
  // entry point for the EvtDeviceAdd callback, KmdfHelloWorldEvtDeviceAdd
  WDF_DRIVER_CONFIG_INIT(&config, KmdfHelloWorldEvtDeviceAdd);

  // Finally, create the driver object
  status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);

  g_DriverObject = DriverObject;

  DriverObject->DriverUnload = DriverUnload;

  return status;
}

NTSTATUS
KmdfHelloWorldEvtDeviceAdd(_In_ WDFDRIVER Driver, _Inout_ PWDFDEVICE_INIT DeviceInit) {
  // We're not using the driver object,
  // so we need to mark it as unreferenced
  UNREFERENCED_PARAMETER(Driver);

  NTSTATUS status;

  // Allocate the device object
  WDFDEVICE hDevice;

  DbgPrintPrefix("EvtDeviceAdd");

  // Create the device object
  status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &hDevice);
  return status;
}