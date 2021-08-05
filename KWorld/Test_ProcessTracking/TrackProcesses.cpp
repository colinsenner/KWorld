#include "TrackProcesses.h"

#include <stdio.h>

// https://www.osronline.com/article.cfm%5Earticle=499.htm
//

static LIST_ENTRY ProcessListHead{&ProcessListHead, &ProcessListHead};

void AddProcess(HANDLE pid) {
  if (ProcessListHead.Flink == NULL || ProcessListHead.Blink == NULL) {
    DbgPrintPrefix("[!] List head not initialized!");
    return;
  }

  PPROCESS_INFO pProc = (PPROCESS_INFO)malloc(sizeof(PROCESS_INFO));

  if (!pProc) {
    DbgPrintPrefix("[!] Problem allocating PROCESS_INFO.");
    return;
  }
  pProc->pid = pid;

  DbgPrintPrefix("[+] Tracking process %llu", (ULONG_PTR)pid);

  InsertHeadList(&ProcessListHead, &pProc->ProcessListEntry);
}

BOOLEAN RemoveProcess(HANDLE pid) {
  if (IsListEmpty(&ProcessListHead)) {
    return FALSE;
  }

  PLIST_ENTRY pEntry = ProcessListHead.Flink;

  while (pEntry != &ProcessListHead) {
    PPROCESS_INFO pProc = CONTAINING_RECORD(pEntry, PROCESS_INFO, ProcessListEntry);

    if (pProc->pid == pid) {
      RemoveEntryList(pEntry);

      printf("[-] Removing process pid: %llu\n", (ULONG_PTR)pProc->pid);
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
    //pEntry = RemoveHeadList(&ProcessListHead);

    PPROCESS_INFO pProc = CONTAINING_RECORD(pEntry, PROCESS_INFO, ProcessListEntry);

    //printf("[-] Removing process pid: %llu\n", (ULONG_PTR)pProc->pid);

    //free(pProc);

    RemoveProcess(pProc->pid);

    pEntry = pEntry->Flink;
  }
}