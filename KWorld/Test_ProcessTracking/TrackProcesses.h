#pragma once
#include <Windows.h>

#include "structures.h"

#define DbgPrintPrefix(s, ...) printf(s "\n", __VA_ARGS__)

typedef struct _THREAD_INFO {
  LIST_ENTRY ThreadEntry;
  HANDLE tid;
} THREAD_INFO, *PTHREAD_INFO;

typedef struct _PROCESS_INFO {
  LIST_ENTRY ProcessEntry;
  HANDLE pid;
  LIST_ENTRY ThreadListHead;
} PROCESS_INFO, *PPROCESS_INFO;

PPROCESS_INFO AddProcess(HANDLE pid);

/// <summary>
/// Safe to call multiple times, even on processes that don't exist
/// </summary>
/// <param name="pid"></param>
/// <returns>TRUE if the process was removed</returns>
BOOLEAN RemoveProcess(HANDLE pid);

void FreeTrackedProcesses();

// Debug
void PrintProcessList();