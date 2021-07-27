#include "ProcessInfos.h"
#include <ntddk.h>

#include "..\Common\DriverCommon.h"

static LIST_ENTRY ProcessListHead;

void InitializeProcessList() {
  InitializeListHead(&ProcessListHead);
}

static PPROCESS_INFO GetProcessEntryByPid(HANDLE pid) {
  PLIST_ENTRY pProcessEntry = ProcessListHead.Flink;

  // Go through all processes we're tracking
  while (pProcessEntry != &ProcessListHead) {
    PPROCESS_INFO pProcInfo = CONTAINING_RECORD(pProcessEntry, PROCESS_INFO, ProcessListEntry);

    if (pProcInfo->pid == pid) {
      return pProcInfo;
    }

    pProcessEntry = pProcessEntry->Flink;
  }

  return NULL;
}

BOOLEAN FreeProcessList() {
  PLIST_ENTRY pEntry = ProcessListHead.Flink;

  while (pEntry && pEntry != &ProcessListHead) {
    PPROCESS_INFO pProcInfo = CONTAINING_RECORD(pEntry, PROCESS_INFO, ProcessListEntry);
    RemoveEntryList(pEntry);
    ExFreePoolWithTag(pProcInfo, DRIVER_POOL_TAG);

    pEntry = pEntry->Flink;
  }

  return TRUE;
}

BOOLEAN RemoveProcessFromList(HANDLE pid) {

  PPROCESS_INFO pProcInfo = GetProcessEntryByPid(pid);

  if (pProcInfo) {
    RemoveEntryList(&pProcInfo->ProcessListEntry);
    ExFreePoolWithTag(pProcInfo, DRIVER_POOL_TAG);

    DbgPrintPrefix("[-] Removed pid %llu", (ULONG_PTR)pid);
    return TRUE;
  }

  return FALSE;
}

void AddProcessToList(HANDLE pid) {
  PPROCESS_INFO pProcInfo;
  pProcInfo = (PPROCESS_INFO)ExAllocatePoolWithTag(PagedPool, sizeof(PROCESS_INFO), DRIVER_POOL_TAG);

  if (pProcInfo) {
    pProcInfo->pid = pid;
    InsertHeadList(&ProcessListHead, &pProcInfo->ProcessListEntry);

    InitializeListHead(&pProcInfo->ThreadListHead);

    DbgPrintPrefix("[+] Added pid %llu", (ULONG_PTR)pid);
  }
}

void AddThreadToProcess(HANDLE pid, HANDLE tid) {
  PPROCESS_INFO pProcInfo = GetProcessEntryByPid(pid);

  if (pProcInfo) {
    DbgPrintPrefix("[+] Fake added tid (%llu) to pid (%llu)", (ULONG_PTR)tid, (ULONG_PTR)pid);
  }
}

BOOLEAN IsProcessInList(HANDLE pid) {
  return GetProcessEntryByPid(pid) != NULL;
}

BOOLEAN IsThreadInProcess(HANDLE tid, HANDLE pid) {
  PLIST_ENTRY pProcessEntry = ProcessListHead.Flink;

  DbgPrintPrefix("Checking if tid (%llu) is in pid (%llu)...", (ULONG_PTR)tid, (ULONG_PTR)pid);

  // Go through all processes we're tracking
  while (pProcessEntry != &ProcessListHead) {
    PPROCESS_INFO pProcInfo = CONTAINING_RECORD(pProcessEntry, PROCESS_INFO, ProcessListEntry);

    if (pProcInfo->pid == pid) {
      // Found the process we're looking for
      // Go through all threads we're tracking
      PLIST_ENTRY pThreadEntry = pProcInfo->ThreadListHead.Flink;

      while (pThreadEntry != &pProcInfo->ThreadListHead) {
        PTHREAD_INFO pThreadInfo = CONTAINING_RECORD(pThreadEntry, THREAD_INFO, ThreadListEntry);

        if (pThreadInfo->ThreadId == tid) {
          DbgPrintPrefix("    Found tid (%llu)", (ULONG_PTR)tid);
          return TRUE;
        }

        pThreadEntry = pThreadEntry->Flink;
      }
    }

    pProcessEntry = pProcessEntry->Flink;
  }

  DbgPrintPrefix("    Not found tid (%llu)", (ULONG_PTR)tid);

  return FALSE;
}
