#pragma once
#include <Windows.h>

#include "structures.h"

#define DbgPrintPrefix(s, ...) printf(s "\n", __VA_ARGS__)

typedef struct _THREAD_INFO {
  LIST_ENTRY ThreadListEntry;
  HANDLE tid;
} THREAD_INFO, *PTHREAD_INFO;

typedef struct _PROCESS_INFO {
  LIST_ENTRY ProcessListEntry;
  HANDLE pid;
  LIST_ENTRY ThreadListHead;
} PROCESS_INFO, *PPROCESS_INFO;

PPROCESS_INFO AddProcess(HANDLE pid);
BOOLEAN RemoveProcess(PPROCESS_INFO pProc);

void FreeTrackedProcesses();