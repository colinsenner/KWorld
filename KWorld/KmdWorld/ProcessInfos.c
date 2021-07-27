#include "ProcessInfos.h"
#include <ntddk.h>

#include "..\Common\DriverCommon.h"

static LIST_ENTRY ProcessListHead;

void InitializeProcessList() {
  InitializeListHead(&ProcessListHead);
}

BOOLEAN FreeProcessList() {
  PLIST_ENTRY pEntry = ProcessListHead.Flink;

  while (pEntry && pEntry != &ProcessListHead) {
    PPROCESS_INFO pProcInfo = CONTAINING_RECORD(pEntry, PROCESS_INFO, ListEntry);
    RemoveEntryList(pEntry);
    ExFreePoolWithTag(pProcInfo, DRIVER_POOL_TAG);

    pEntry = pEntry->Flink;
  }

  return TRUE;
}

BOOLEAN RemoveProcessFromList(HANDLE pid) {
  PLIST_ENTRY pEntry = ProcessListHead.Flink;

  while (pEntry && pEntry != &ProcessListHead) {
    PPROCESS_INFO pProcInfo = CONTAINING_RECORD(pEntry, PROCESS_INFO, ListEntry);

    if (pProcInfo->pid == pid) {
      RemoveEntryList(pEntry);
      ExFreePoolWithTag(pProcInfo, DRIVER_POOL_TAG);

      DbgPrintPrefix("[-] Removed pid %llu", (ULONG_PTR)pid);

      return TRUE;
    }

    pEntry = pEntry->Flink;
  }

  return TRUE;
}

void AddProcessToList(HANDLE pid) {
  PPROCESS_INFO pProcInfo;
  pProcInfo = (PPROCESS_INFO)ExAllocatePoolWithTag(PagedPool, sizeof(PROCESS_INFO), DRIVER_POOL_TAG);

  if (pProcInfo) {
    pProcInfo->pid = pid;
    InsertHeadList(&ProcessListHead, &pProcInfo->ListEntry);

    DbgPrintPrefix("[+] Added pid %llu", (ULONG_PTR)pid);
  }
}

BOOLEAN IsProcessInList(HANDLE pid) {
  PLIST_ENTRY pEntry = ProcessListHead.Flink;

  while (pEntry && pEntry != &ProcessListHead) {
    PPROCESS_INFO pProcInfo = CONTAINING_RECORD(pEntry, PROCESS_INFO, ListEntry);

    if (pProcInfo->pid == pid) {
      return TRUE;
    }

    pEntry = pEntry->Flink;
  }

  return FALSE;
}
