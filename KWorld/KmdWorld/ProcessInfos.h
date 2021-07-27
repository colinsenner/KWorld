#pragma once
#include <ntddk.h>

typedef struct _THREAD_INFO {
  LIST_ENTRY ThreadListEntry;
  HANDLE ThreadId;
} THREAD_INFO, *PTHREAD_INFO;

typedef struct _PROCESS_INFO {
  LIST_ENTRY ProcessListEntry;
  HANDLE pid;
  LIST_ENTRY ThreadListHead;
} PROCESS_INFO, *PPROCESS_INFO;

void InitializeProcessList();
void AddProcessToList(HANDLE pid);
void AddThreadToProcess(HANDLE pid, HANDLE tid);
BOOLEAN RemoveProcessFromList(HANDLE pid);
BOOLEAN IsProcessInList(HANDLE pid);
BOOLEAN IsThreadInProcess(HANDLE tid, HANDLE pid);
BOOLEAN FreeProcessList();
