#include "ProcessInfos.h"

#include <ntddk.h>

#include "..\Common\DriverCommon.h"

typedef struct _THREAD_INFO {
  LIST_ENTRY ThreadEntry;
  HANDLE tid;
} THREAD_INFO, *PTHREAD_INFO;

typedef struct _PROCESS_INFO {
  LIST_ENTRY ProcessEntry;
  HANDLE pid;
  LIST_ENTRY ThreadListHead;
} PROCESS_INFO, *PPROCESS_INFO;

// global list head to track all processes
static LIST_ENTRY ProcessListHead;

void InitializeProcessList() {
  InitializeListHead(&ProcessListHead);
}

PPROCESS_INFO GetProcessEntryByPid(HANDLE pid) {
  PLIST_ENTRY pEntry = ProcessListHead.Flink;

  while (pEntry != &ProcessListHead) {
    PPROCESS_INFO pProcInfo = CONTAINING_RECORD(pEntry, PROCESS_INFO, ProcessListEntry);

    if (pProcInfo->pid == pid) {
      return pProcInfo;
    }


    pEntry = pEntry->Flink;
  }

  return NULL;
}

PTHREAD_INFO GetThreadEntryByTid(PPROCESS_INFO pProc, HANDLE tid) {
  if (pProc != NULL) {
    PLIST_ENTRY pEntry = pProc->ThreadListHead.Flink;

    while (pEntry != &pProc->ThreadListHead) {
      PTHREAD_INFO pThreadInfo = CONTAINING_RECORD(pEntry, THREAD_INFO, ThreadListEntry);

      if (pThreadInfo == NULL) {
        DbgPrintPrefix("this should be impossible?");
        return NULL;
      }

      if (pThreadInfo->tid == tid) {
        return pThreadInfo;
      }

      pEntry = pEntry->Flink;
    }
  }

  return NULL;
}

int TotalThreadsInProcess(HANDLE pid) {
  PPROCESS_INFO pProcInfo = GetProcessEntryByPid(pid);
  int result = 0;

  if (pProcInfo) {
    PLIST_ENTRY pThreadEntry = pProcInfo->ThreadListHead.Flink;

    while (pThreadEntry != &pProcInfo->ThreadListHead) {
      result++;
      pThreadEntry = pThreadEntry->Flink;
    }
  }

  return result;
}

BOOLEAN FreeThreadList(PPROCESS_INFO pProc) {
  if (pProc) {
    PLIST_ENTRY pThreadEntry = pProc->ThreadListHead.Flink;

    while (pThreadEntry != &pProc->ThreadListHead) {
      PTHREAD_INFO pThreadInfo = CONTAINING_RECORD(pThreadEntry, THREAD_INFO, ThreadListEntry);

      DbgPrintPrefix("  [-] Freed thread entry tid (%llu)", (ULONG_PTR)pThreadInfo->tid);

      RemoveEntryList(pThreadEntry);
      ExFreePoolWithTag(pThreadInfo, DRIVER_POOL_TAG);

      pThreadEntry = pThreadEntry->Flink;
    }

    return TRUE;
  }

  return FALSE;
}

void FreeProcessList() {
  PLIST_ENTRY pEntry = ProcessListHead.Flink;

  while (pEntry != &ProcessListHead) {
    PPROCESS_INFO pProcInfo = CONTAINING_RECORD(pEntry, PROCESS_INFO, ProcessListEntry);

    FreeThreadList(pProcInfo);

    RemoveEntryList(pEntry);
    ExFreePoolWithTag(pProcInfo, DRIVER_POOL_TAG);

    pEntry = pEntry->Flink;
  }
}

BOOLEAN RemoveProcessFromList(HANDLE pid) {
  PPROCESS_INFO pProcInfo = GetProcessEntryByPid(pid);

  if (pProcInfo != NULL) {
    RemoveEntryList(&pProcInfo->ProcessEntry);
    ExFreePoolWithTag(pProcInfo, DRIVER_POOL_TAG);

    //DbgPrintPrefix("[-] Process removed pid %llu", (ULONG_PTR)pid);
    return TRUE;
  }

  return FALSE;
}

BOOLEAN RemoveThreadFromProcess(HANDLE pid, HANDLE tid) {
  PTHREAD_INFO pThreadInfo = GetThreadEntryByTid(GetProcessEntryByPid(pid), tid);

  if (pThreadInfo != NULL) {
    RemoveEntryList(&pThreadInfo->ThreadEntry);
    ExFreePoolWithTag(pThreadInfo, DRIVER_POOL_TAG);

    DbgPrintPrefix("  [-] Thread removed tid (%llu) from pid (%llu) (total: %d)", (ULONG_PTR)tid, (ULONG_PTR)pid, TotalThreadsInProcess(pid));
    return TRUE;
  }

  return FALSE;
}

BOOLEAN AddProcessToList(HANDLE pid) {
  if (!IsProcessInList(pid)) {
    PPROCESS_INFO pProcInfo = (PPROCESS_INFO)ExAllocatePoolWithTag(PagedPool, sizeof(PROCESS_INFO), DRIVER_POOL_TAG);

    if (pProcInfo) {
      pProcInfo->pid = pid;
      InsertHeadList(&ProcessListHead, &pProcInfo->ProcessEntry);

      InitializeListHead(&pProcInfo->ThreadListHead);

      DbgPrintPrefix("[+] Process added pid %llu", (ULONG_PTR)pid);
      return TRUE;
    }
  }

  return FALSE;
}

BOOLEAN AddThreadToProcess(HANDLE pid, HANDLE tid) {
  PPROCESS_INFO pProc = GetProcessEntryByPid(pid);

  if (pProc != NULL) {
    if (!IsThreadInProcess(tid, pid)) {
      PTHREAD_INFO pThreadInfo = (PTHREAD_INFO)ExAllocatePoolWithTag(PagedPool, sizeof(THREAD_INFO), DRIVER_POOL_TAG);

      if (pThreadInfo) {
        pThreadInfo->tid = tid;

        InsertHeadList(&pProc->ThreadListHead, &pThreadInfo->ThreadEntry);
        DbgPrintPrefix("  [+] Thread added tid (%llu) to pid (%llu) (total: %d)", (ULONG_PTR)tid, (ULONG_PTR)pid,
                       TotalThreadsInProcess(pid));

        return TRUE;
      }
    }
  }

  return FALSE;
}

BOOLEAN IsProcessInList(HANDLE pid) {
  return GetProcessEntryByPid(pid) != NULL;
}

BOOLEAN IsThreadInProcess(HANDLE tid, HANDLE pid) {
  return GetThreadEntryByTid(GetProcessEntryByPid(pid), tid) != NULL;
}