#pragma once

// IOCTL Codes
#define IOCTL_THREAD_UNHIDE_FROM_DEBUGGER \
  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2049, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Structs to pass data to kernel <=> userland
typedef struct _ProcessData {
  ULONG ProcessId;
} ProcessData, *PProcessData;

typedef struct _ProcessDataComplete {
  ULONG NumThreadsUnhidden;
} ProcessDataComplete, *PProcessDataComplete;