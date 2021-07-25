#include <ntifs.h>
#include <ntddk.h>
#include <wdf.h>

#include "..\Common\DriverCommon.h"

DRIVER_INITIALIZE DriverEntry;

UNICODE_STRING DEVICE_NAME = RTL_CONSTANT_STRING(L"\\Device\\KmdWorld");
UNICODE_STRING DEVICE_SYMBOLIC_NAME = RTL_CONSTANT_STRING(L"\\??\\KmdWorld");

//
// Step 1: Pass pid to kernel and return success or failure
// Step 2: Lookup process by pid
// Step 3: Lookup structure offsets
//

void DriverUnload(PDRIVER_OBJECT DriverObject) {
  DbgPrintPrefix("Driver unloaded, deleting symbolic links and devices");
  IoDeleteDevice(DriverObject->DeviceObject);
  IoDeleteSymbolicLink(&DEVICE_SYMBOLIC_NAME);
}

NTSTATUS ThreadUnhideFromDebugger(size_t pid) {
  NTSTATUS status = STATUS_SUCCESS;

  PEPROCESS pProcess = NULL;

  status = PsLookupProcessByProcessId((HANDLE)pid, &pProcess);

  if (status != STATUS_SUCCESS) {
    DbgPrintPrefix("Couldn't find the process by pid %zu. Status: 0x%X", pid, status);
    return status;
  }

  DbgPrintPrefix("Found process %p", pProcess);

  ASSERT(pProcess);

  // Calculate
  PLIST_ENTRY pThreadListHead = (PLIST_ENTRY)ADDPTR(pProcess, 0x5e0);

  ASSERT(pThreadListHead);

  DbgPrintPrefix("ThreadListHead: %p", pThreadListHead);

  PLIST_ENTRY entry = pThreadListHead;

  while (entry != pThreadListHead->Flink) {
    entry = entry->Flink;

    // Calculate
    PVOID pThread = SUBPTR(entry, 0x4e8);

    ASSERT(pThread);

    DbgPrintPrefix("  Thread: %p", pThread);

    // offset: 0x510 on ETHREAD
    PULONG CrossThreadFlags = (PULONG)ADDPTR(pThread, 0x510);

    DbgPrintPrefix("    CrossThreadFlags: 0x%x", *CrossThreadFlags);

    if (CHECK_BIT(*CrossThreadFlags, 2)) {
      DbgPrintPrefix("    HideFromDebugger: Set");

      // Unset HideFromDebugger
      *CrossThreadFlags = CLEAR_BIT(*CrossThreadFlags, 2);

      DbgPrintPrefix("    HideFromDebugger: Removed");
    }
  }

  return status;
}

NTSTATUS HandleCustomIOCTL(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
  UNREFERENCED_PARAMETER(DeviceObject);
  PIO_STACK_LOCATION stackLocation = NULL;

  NTSTATUS status = STATUS_SUCCESS;

  stackLocation = IoGetCurrentIrpStackLocation(Irp);

  if (stackLocation->Parameters.DeviceIoControl.IoControlCode == IOCTL_THREAD_UNHIDE_FROM_DEBUGGER) {
    DbgPrintPrefix("IOCTL_THREAD_UNHIDE_FROM_DEBUGGER (0x%x) issued",
                   stackLocation->Parameters.DeviceIoControl.IoControlCode);

    ASSERT(stackLocation->Parameters.DeviceIoControl.InputBufferLength == sizeof(size_t));
    ASSERT(Irp->AssociatedIrp.SystemBuffer);

    size_t pid = *(size_t*)Irp->AssociatedIrp.SystemBuffer;
    DbgPrintPrefix("PID from userland: %zu", pid);

    status = ThreadUnhideFromDebugger(pid);
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
  UNREFERENCED_PARAMETER(DriverObject);
  UNREFERENCED_PARAMETER(RegistryPath);

  NTSTATUS status = 0;

  // routine that will execute when our driver is unloaded/service is stopped
  DriverObject->DriverUnload = DriverUnload;

  // routine for handling IO requests from userland
  DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HandleCustomIOCTL;

  // routines that will execute once a handle to our device's symbolik link is opened/closed
  DriverObject->MajorFunction[IRP_MJ_CREATE] = HandleIrpCreateClose;
  DriverObject->MajorFunction[IRP_MJ_CLOSE] = HandleIrpCreateClose;

  DbgPrintPrefix("Driver loaded");

  IoCreateDevice(DriverObject, 0, &DEVICE_NAME, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE,
                 &DriverObject->DeviceObject);
  if (!NT_SUCCESS(status)) {
    DbgPrintPrefix("Could not create device %wZ", DEVICE_NAME);
  } else {
    DbgPrintPrefix("Device %wZ created", DEVICE_NAME);
  }

  status = IoCreateSymbolicLink(&DEVICE_SYMBOLIC_NAME, &DEVICE_NAME);
  if (NT_SUCCESS(status)) {
    DbgPrintPrefix("Symbolic link %wZ created", DEVICE_SYMBOLIC_NAME);
  } else {
    DbgPrintPrefix("Error creating symbolic link %wZ", DEVICE_SYMBOLIC_NAME);
  }

  return STATUS_SUCCESS;
}
