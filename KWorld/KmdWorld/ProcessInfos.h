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

// Call before any other functions
void InitializeProcessList();

// Adds a pid to the process list, safe to call multiple times.
// Returns TRUE if the process was added.
BOOLEAN AddProcessToList(HANDLE pid);

// Adds a tid to the tracked process with pid, safe to call multiple times.
// Returns TRUE if the thread id was added.
BOOLEAN AddThreadToProcess(HANDLE pid, HANDLE tid);

// Removes the process with pid from the list, safe to call multiple times.
// Returns TRUE if the process was removed.
BOOLEAN RemoveProcessFromList(HANDLE pid);

// Removes a thread from the process's thread list, safe to call multiple times
// Returns TRUE if the thread id was removed.
BOOLEAN RemoveThreadFromProcess(HANDLE pid, HANDLE tid);

// Frees the entire tracked process list including all tracked threads.
void FreeProcessList();
