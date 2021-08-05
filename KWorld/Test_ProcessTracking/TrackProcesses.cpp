#include "TrackProcesses.h"

// https://www.osronline.com/article.cfm%5Earticle=499.htm

static LIST_ENTRY ProcessListHead;

void InitializeTrackedProcesses() {
  InitializeListHead(&ProcessListHead);
}

PPROCESS_INFO AddProcess(HANDLE pid) {
  if (ProcessListHead.Flink == NULL || ProcessListHead.Blink == NULL) {
    DbgPrintPrefix("[!] List head not initialized!");
    return NULL;
  }

  // Create the process_info object
  PPROCESS_INFO pProc = (PPROCESS_INFO)ExAllocatePoolWithTag(PagedPool, sizeof(PROCESS_INFO), DRIVER_POOL_TAG);

  if (!pProc) {
    DbgPrintPrefix("[!] Problem allocating PROCESS_INFO.");
    return NULL;
  }

  // Set members
  pProc->pid = pid;

  // Init Thread List Head
  InitializeListHead(&pProc->ThreadListHead);

  DbgPrintPrefix("[+] Tracking process %llu", (ULONG_PTR)pid);

  InsertHeadList(&ProcessListHead, &pProc->ProcessEntry);

  return pProc;
}

static PPROCESS_INFO FindProcess(HANDLE pid) {
  PLIST_ENTRY pEntry = ProcessListHead.Flink;

  while (pEntry != &ProcessListHead) {
    PPROCESS_INFO pProc = CONTAINING_RECORD(pEntry, PROCESS_INFO, ProcessEntry);

    if (pProc->pid == pid) {
      return pProc;
    }

    pEntry = pEntry->Flink;
  }
  return NULL;
}

PTHREAD_INFO AddThreadToProcess(HANDLE pid, HANDLE tid) {
  PPROCESS_INFO pProc = FindProcess(pid);

  if (!pProc) {
    return NULL;
  }

  if (pProc->ThreadListHead.Flink == NULL || pProc->ThreadListHead.Blink == NULL) {
    DbgPrintPrefix("[!] Thread List head not initialized!");
    return NULL;
  }

  // Create the thread info object
  PTHREAD_INFO pThread = (PTHREAD_INFO)ExAllocatePoolWithTag(PagedPool, sizeof(THREAD_INFO), DRIVER_POOL_TAG);

  if (!pThread) {
    DbgPrintPrefix("[!] Problem allocating THREAD_INFO.");
    return NULL;
  }

  // Set members
  pThread->tid = tid;

  DbgPrintPrefix("  [+] Add thread %llu to process %llu", (ULONG_PTR)tid, (ULONG_PTR)pid);

  InsertHeadList(&pProc->ThreadListHead, &pThread->ThreadEntry);

  return pThread;
}

BOOLEAN RemoveThreadFromProcess(HANDLE pid, HANDLE tid) {
  PPROCESS_INFO pProc = FindProcess(pid);

  if (!pProc) {
    return FALSE;
  }

  PLIST_ENTRY pEntry = pProc->ThreadListHead.Flink;

  while (pEntry != &pProc->ThreadListHead) {
    PTHREAD_INFO pThread = CONTAINING_RECORD(pEntry, THREAD_INFO, ThreadEntry);

    if (pThread->tid == tid) {
      DbgPrintPrefix("  [-] Removing thread %llu from process %llu", (ULONG_PTR)tid, (ULONG_PTR)pid);

      RemoveEntryList(&pThread->ThreadEntry);
      ExFreePoolWithTag(pThread, DRIVER_POOL_TAG);

      return TRUE;
    }

    pEntry = pEntry->Flink;
  }

  return FALSE;
}

static void FreeThreadsFromProcess(PPROCESS_INFO pProc) {
  if (IsListEmpty(&pProc->ThreadListHead)) {
    return;
  }

  PLIST_ENTRY pEntry = pProc->ThreadListHead.Flink;

  while (!IsListEmpty(&pProc->ThreadListHead)) {
    PTHREAD_INFO pThread = CONTAINING_RECORD(pEntry, THREAD_INFO, ThreadEntry);

    pEntry = pEntry->Flink;

    BOOLEAN removed = RemoveThreadFromProcess(pProc->pid, pThread->tid);

    // ASSERT(was_removed)
    if (!removed) {
      DbgPrintPrefix("!!! ASSERT NOTHING WAS REMOVED !!!");
      return;
    }
  }
}

BOOLEAN RemoveProcess(HANDLE pid) {
  if (IsListEmpty(&ProcessListHead)) {
    return FALSE;
  }

  PLIST_ENTRY pEntry = ProcessListHead.Flink;

  while (pEntry != &ProcessListHead) {
    PPROCESS_INFO pProc = CONTAINING_RECORD(pEntry, PROCESS_INFO, ProcessEntry);

    if (pProc->pid == pid) {
      DbgPrintPrefix("[-] Removing process pid: %llu", (ULONG_PTR)pProc->pid);

      FreeThreadsFromProcess(pProc);

      RemoveEntryList(&pProc->ProcessEntry);
      ExFreePoolWithTag(pProc, DRIVER_POOL_TAG);

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
      DbgPrintPrefix("!!! ASSERT NOTHING WAS REMOVED !!!");
      return;
    }
  }
}

BOOLEAN IsProcessTracked(HANDLE pid) {
  PLIST_ENTRY pEntry = ProcessListHead.Flink;
  while (pEntry != &ProcessListHead) {
    PPROCESS_INFO pProc = CONTAINING_RECORD(pEntry, PROCESS_INFO, ProcessEntry);

    if (pProc->pid == pid)
      return TRUE;

    pEntry = pEntry->Flink;
  }
  return FALSE;
}

static void PrintThreadList(PLIST_ENTRY head) {
  if (IsListEmpty(head)) {
    return;
  }

  PLIST_ENTRY pEntry = head->Flink;
  while (pEntry != head) {
    PTHREAD_INFO pThread = CONTAINING_RECORD(pEntry, THREAD_INFO, ThreadEntry);

    DbgPrintPrefix("    tid: %llu", (ULONG_PTR)pThread->tid);

    pEntry = pEntry->Flink;
  }
}

void PrintProcessList() {
  if (IsListEmpty(&ProcessListHead)) {
    return;
  }

  DbgPrintPrefix("====================");

  PLIST_ENTRY pEntry = ProcessListHead.Flink;
  while (pEntry != &ProcessListHead) {
    PPROCESS_INFO pProc = CONTAINING_RECORD(pEntry, PROCESS_INFO, ProcessEntry);

    DbgPrintPrefix("Process: %llu", (ULONG_PTR)pProc->pid);

    PrintThreadList(&pProc->ThreadListHead);

    pEntry = pEntry->Flink;
  }

  DbgPrintPrefix("====================");
}
