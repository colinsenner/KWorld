#include "Driver.h"

#include <ntifs.h>
#include <ntddk.h>

#include "..\Common\DriverCommon.h"
#include "ThreadUnhideFromDebugger.h"
#include "undocumented.h"

UNICODE_STRING DEVICE_NAME = RTL_CONSTANT_STRING(L"\\Device\\KmdWorld");
UNICODE_STRING DEVICE_SYMBOLIC_NAME = RTL_CONSTANT_STRING(L"\\??\\KmdWorld");

void KmdWorldUnload(PDRIVER_OBJECT DriverObject) {
  DbgPrintPrefix("Driver unloaded, deleting symbolic links and devices");
  IoDeleteDevice(DriverObject->DeviceObject);
  IoDeleteSymbolicLink(&DEVICE_SYMBOLIC_NAME);
}

NTSTATUS KmdWorldIoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
  UNREFERENCED_PARAMETER(DeviceObject);
  PIO_STACK_LOCATION stackLocation = NULL;

  NTSTATUS status = STATUS_SUCCESS;

  stackLocation = IoGetCurrentIrpStackLocation(Irp);
  ULONG IoControlCode = stackLocation->Parameters.DeviceIoControl.IoControlCode;

  switch (IoControlCode) {
    case IOCTL_THREAD_UNHIDE_FROM_DEBUGGER:
      DbgPrintPrefix("IOCTL_THREAD_UNHIDE_FROM_DEBUGGER (0x%x) issued",
                     stackLocation->Parameters.DeviceIoControl.IoControlCode);

      if (stackLocation->Parameters.DeviceIoControl.InputBufferLength < sizeof(ProcessData)) {
        status = STATUS_BUFFER_TOO_SMALL;
        break;
      }

      ASSERT(Irp->AssociatedIrp.SystemBuffer);

      auto data = *(ProcessData*)Irp->AssociatedIrp.SystemBuffer;

      status = ThreadUnhideFromDebugger(data);

      // HANDLE pid = *(HANDLE*)Irp->AssociatedIrp.SystemBuffer;
      // DbgPrintPrefix("PID from userland: %llu", pid);

      // if (pid <= 4) {
      //  DbgPrintPrefix("Not allowed on system process");
      //  break;
      //}

      // status = ThreadUnhideFromDebugger(pid);
      break;
    default:
      status = STATUS_INVALID_DEVICE_REQUEST;
      break;
  }

  // How many bytes we're returning to userland
  Irp->IoStatus.Status = status;
  Irp->IoStatus.Information = 0;
  IoCompleteRequest(Irp, IO_NO_INCREMENT);
  return status;
}

NTSTATUS KmdWorldCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
  UNREFERENCED_PARAMETER(DeviceObject);

  PIO_STACK_LOCATION stackLocation = NULL;
  stackLocation = IoGetCurrentIrpStackLocation(Irp);

  switch (stackLocation->MajorFunction) {
    case IRP_MJ_CREATE:
      DbgPrintPrefix("Handle to symbolink link %wZ opened", DEVICE_SYMBOLIC_NAME);
      break;
    case IRP_MJ_CLOSE:
      DbgPrintPrefix("Handle to symbolink link %wZ closed", DEVICE_SYMBOLIC_NAME);
      break;
    default:
      break;
  }

  Irp->IoStatus.Information = 0;
  Irp->IoStatus.Status = STATUS_SUCCESS;
  IoCompleteRequest(Irp, IO_NO_INCREMENT);

  return STATUS_SUCCESS;
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
  UNREFERENCED_PARAMETER(RegistryPath);

  NTSTATUS status = STATUS_SUCCESS;

  // routine that will execute when our driver is unloaded/service is stopped
  DriverObject->DriverUnload = KmdWorldUnload;

  // routine for handling IO requests from userland
  DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = KmdWorldIoControl;

  // routines that will execute once a handle to our device's symbolik link is opened/closed
  DriverObject->MajorFunction[IRP_MJ_CREATE] = KmdWorldCreateClose;
  DriverObject->MajorFunction[IRP_MJ_CLOSE] = KmdWorldCreateClose;

  DbgPrintPrefix("Driver loaded");

  // Create the device
  status = IoCreateDevice(DriverObject, 0, &DEVICE_NAME, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE,
                          &DriverObject->DeviceObject);

  if (NT_SUCCESS(status)) {
    DbgPrintPrefix("Device %wZ created", DEVICE_NAME);
  } else {
    DbgPrintPrefix("Could not create device %wZ", DEVICE_NAME);
    return status;
  }

  status = IoCreateSymbolicLink(&DEVICE_SYMBOLIC_NAME, &DEVICE_NAME);

  if (NT_SUCCESS(status)) {
    DbgPrintPrefix("Symbolic link %wZ created", DEVICE_SYMBOLIC_NAME);
  } else {
    DbgPrintPrefix("Error creating symbolic link %wZ", DEVICE_SYMBOLIC_NAME);
    return status;
  }

  status = InitUndocumented();

  if (!NT_SUCCESS(status)) {
    DbgPrintPrefix("Error while initializing undocumented functions");
    return status;
  }

  return STATUS_SUCCESS;
}