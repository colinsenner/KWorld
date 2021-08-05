#include "TrackProcesses.h"

#include <stdio.h>

// https://www.osronline.com/article.cfm%5Earticle=499.htm
//

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

  InsertHeadList(&ProcessListHead, &pProc->ProcessListEntry);

  return pProc;
}

BOOLEAN RemoveProcess(PPROCESS_INFO pProc) {
  if (IsListEmpty(&ProcessListHead)) {
    return NULL;
  }

  RemoveEntryList(&pProc->ProcessListEntry);

  printf("[-] Removing process pid: %llu\n", (ULONG_PTR)pProc->pid);
  free(pProc);

  return FALSE;
}

void FreeTrackedProcesses() {
  if (IsListEmpty(&ProcessListHead)) {
    return;
  }

  PLIST_ENTRY pEntry = ProcessListHead.Flink;

  while (!IsListEmpty(&ProcessListHead)) {
    PPROCESS_INFO pProc = CONTAINING_RECORD(pEntry, PROCESS_INFO, ProcessListEntry);

    pEntry = pEntry->Flink;

    RemoveProcess(pProc);
  }
}