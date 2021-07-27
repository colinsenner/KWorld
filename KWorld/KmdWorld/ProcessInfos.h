#pragma once
#include <ntddk.h>

typedef struct _THREAD_INFO {
  HANDLE ThreadId;
} THREAD_INFO;

typedef struct _PROCESS_INFO {
  LIST_ENTRY ListEntry;
  HANDLE pid;
} PROCESS_INFO, *PPROCESS_INFO;

void InitializeProcessList();
void AddProcessToList(HANDLE pid);
BOOLEAN RemoveProcessFromList(HANDLE pid);
BOOLEAN IsProcessInList(HANDLE pid);
BOOLEAN FreeProcessList();
