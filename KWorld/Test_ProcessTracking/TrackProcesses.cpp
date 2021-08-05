#include "TrackProcesses.h"

#include <stdio.h>

// https://www.osronline.com/article.cfm%5Earticle=499.htm


static LIST_ENTRY ProcessListHead{&ProcessListHead, &ProcessListHead};

PPROCESS_INFO AddProcess(HANDLE pid) {
  if (ProcessListHead.Flink == NULL || ProcessListHead.Blink == NULL) {
    DbgPrintPrefix("[!] List head not initialized!");
    return NULL;
  }

  PPROCESS_INFO pProc = (PPROCESS_INFO)malloc(sizeof(PROCESS_INFO));

  if (!pProc) {
    DbgPrintPrefix("[!] Problem allocating PROCESS_INFO.");
    return NULL;
  }
  pProc->pid = pid;

  DbgPrintPrefix("[+] Tracking process %llu", (ULONG_PTR)pid);

  InsertHeadList(&ProcessListHead, &pProc->ProcessEntry);

  return pProc;
}

BOOLEAN RemoveProcess(HANDLE pid) {
  if (IsListEmpty(&ProcessListHead)) {
    return NULL;
  }

  PLIST_ENTRY pEntry = ProcessListHead.Flink;

  while (pEntry != &ProcessListHead) {
    PPROCESS_INFO pProc = CONTAINING_RECORD(pEntry, PROCESS_INFO, ProcessEntry);

    if (pProc->pid == pid) {
      printf("[-] Removing process pid: %llu\n", (ULONG_PTR)pProc->pid);

      RemoveEntryList(&pProc->ProcessEntry);
      free(pProc);

      return TRUE;
    }

    pEntry = pEntry->Flink;
  }

  return FALSE;
}

void FreeTrackedProcesses() {
  if (IsListEmpty(&ProcessListHead)) {
    return;
  }

  PLIST_ENTRY pEntry = ProcessListHead.Flink;

  while (!IsListEmpty(&ProcessListHead)) {
    PPROCESS_INFO pProc = CONTAINING_RECORD(pEntry, PROCESS_INFO, ProcessEntry);

    pEntry = pEntry->Flink;

    BOOLEAN removed = RemoveProcess(pProc->pid);

    // ASSERT(was_removed)
    if (!removed) {
      printf("!!! ASSERT NOTHING WAS REMOVED !!!");
      return;
    }
  }
}

void PrintProcessList() {
  if (IsListEmpty(&ProcessListHead)) {
    return;
  }

  printf("======== PROC LIST ========\n");

  PLIST_ENTRY pEntry = ProcessListHead.Flink;
  while (pEntry != &ProcessListHead) {
    PPROCESS_INFO pProc = CONTAINING_RECORD(pEntry, PROCESS_INFO, ProcessEntry);

    printf("Process with pid: %llu\n", (ULONG_PTR)pProc->pid);

    pEntry = pEntry->Flink;
  }

  printf("===========================\n");
}
