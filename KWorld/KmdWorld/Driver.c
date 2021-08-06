#include <ntifs.h>
#include <ntddk.h>

#include "..\Common\DriverCommon.h"
#include "UtilNt.h"
#include "undocumented.h"

DRIVER_INITIALIZE DriverEntry;

UNICODE_STRING DEVICE_NAME = RTL_CONSTANT_STRING(L"\\Device\\KmdWorld");
UNICODE_STRING DEVICE_SYMBOLIC_NAME = RTL_CONSTANT_STRING(L"\\??\\KmdWorld");

void DriverUnload(PDRIVER_OBJECT DriverObject) {
  DbgPrintPrefix("Driver unloaded, deleting symbolic links and devices");
  IoDeleteDevice(DriverObject->DeviceObject);
  IoDeleteSymbolicLink(&DEVICE_SYMBOLIC_NAME);
}

NTSTATUS ThreadUnhideFromDebugger(size_t pid) {
  UNREFERENCED_PARAMETER(pid);

  NTSTATUS status = STATUS_SUCCESS;

  // Find the process
  SYSTEM_PROCESS_INFORMATION spi;
  status = GetSystemProcessInfo((HANDLE)pid, &spi);

  if (!NT_SUCCESS(status)) {
    DbgPrintPrefix("Couldn't find the process %llu. (0x%X)", (ULONG_PTR)pid, status);
    return status;
  }

  DbgPrintPrefix("Total threads: %lu", spi.NumberOfThreads);

  // Find the threads associated with this process
  for (ULONG thread_index = 0; thread_index < spi.NumberOfThreads; ++thread_index) {
    PSYSTEM_THREAD_INFORMATION ti = &spi.Threads[thread_index];
    if (ti->ClientId.UniqueProcess != spi.UniqueProcessId)
      continue;

    DbgPrintPrefix("tid: %llu", (ULONG_PTR)ti->ClientId.UniqueThread);
  }

  return status;
}

NTSTATUS HandleCustomIOCTL(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
  UNREFERENCED_PARAMETER(DeviceObject);
  PIO_STACK_LOCATION stackLocation = NULL;

  NTSTATUS status = STATUS_SUCCESS;

  stackLocation = IoGetCurrentIrpStackLocation(Irp);
  ULONG IoControlCode = stackLocation->Parameters.DeviceIoControl.IoControlCode;

  switch (IoControlCode) {
    case IOCTL_THREAD_UNHIDE_FROM_DEBUGGER:
      DbgPrintPrefix("IOCTL_THREAD_UNHIDE_FROM_DEBUGGER (0x%x) issued",
                     stackLocation->Parameters.DeviceIoControl.IoControlCode);

      ASSERT(stackLocation->Parameters.DeviceIoControl.InputBufferLength == sizeof(size_t));
      ASSERT(Irp->AssociatedIrp.SystemBuffer);

      size_t pid = *(size_t*)Irp->AssociatedIrp.SystemBuffer;
      DbgPrintPrefix("PID from userland: %zu", pid);

      status = ThreadUnhideFromDebugger(pid);
      break;
    default:
      DbgPrintPrefix("Unsupported IOCTL: (0x%x)", IoControlCode);
      break;
  }

  // How many bytes we're returning to userland
  Irp->IoStatus.Information = 0;
  Irp->IoStatus.Status = status;

  IoCompleteRequest(Irp, IO_NO_INCREMENT);

  return status;
}

NTSTATUS HandleIrpCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
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

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
  UNREFERENCED_PARAMETER(RegistryPath);

  NTSTATUS status = STATUS_SUCCESS;

  // routine that will execute when our driver is unloaded/service is stopped
  DriverObject->DriverUnload = DriverUnload;

  // routine for handling IO requests from userland
  DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HandleCustomIOCTL;

  // routines that will execute once a handle to our device's symbolik link is opened/closed
  DriverObject->MajorFunction[IRP_MJ_CREATE] = HandleIrpCreateClose;
  DriverObject->MajorFunction[IRP_MJ_CLOSE] = HandleIrpCreateClose;

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
