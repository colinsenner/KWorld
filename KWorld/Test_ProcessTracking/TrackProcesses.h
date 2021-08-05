#pragma once
#include <Windows.h>

#include "structures.h"

typedef struct _THREAD_INFO {
  LIST_ENTRY ThreadEntry;
  HANDLE tid;
} THREAD_INFO, *PTHREAD_INFO;

typedef struct _PROCESS_INFO {
  LIST_ENTRY ProcessEntry;
  HANDLE pid;
  LIST_ENTRY ThreadListHead;
} PROCESS_INFO, *PPROCESS_INFO;

void InitializeTrackedProcesses();

PPROCESS_INFO AddProcess(HANDLE pid);

PTHREAD_INFO AddThreadToProcess(HANDLE pid, HANDLE tid);

BOOLEAN RemoveProcess(HANDLE pid);

BOOLEAN RemoveThreadFromProcess(HANDLE pid, HANDLE tid);

void FreeTrackedProcesses();

BOOLEAN IsProcessTracked(HANDLE pid);

// Debug
void PrintProcessList();