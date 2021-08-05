#pragma once
#include <Windows.h>
#include <stdio.h>

#define DbgPrintPrefix(s, ...) printf(s "\n", __VA_ARGS__)

FORCEINLINE VOID InitializeListHead(_Out_ PLIST_ENTRY ListHead)
{
  ListHead->Flink = ListHead->Blink = ListHead;
  return;
}

FORCEINLINE VOID InsertHeadList(_Inout_ PLIST_ENTRY ListHead, _Out_ PLIST_ENTRY Entry)
{
  PLIST_ENTRY NextEntry;

  NextEntry = ListHead->Flink;

  if (NextEntry->Blink != ListHead) {
    printf("!!!FATAL LIST ERROR!!!\n");
    return;
  }

  Entry->Flink = NextEntry;
  Entry->Blink = ListHead;
  NextEntry->Blink = Entry;
  ListHead->Flink = Entry;
  return;
}

BOOLEAN CFORCEINLINE IsListEmpty(_In_ const LIST_ENTRY* ListHead)

{
  return (BOOLEAN)(ListHead->Flink == ListHead);
}

FORCEINLINE PLIST_ENTRY RemoveHeadList(_Inout_ PLIST_ENTRY ListHead)
{
  PLIST_ENTRY Entry;
  PLIST_ENTRY NextEntry;

  Entry = ListHead->Flink;

  NextEntry = Entry->Flink;
  if ((Entry->Blink != ListHead) || (NextEntry->Blink != Entry)) {
    printf("!!!FATAL LIST ERROR!!!\n");
    return NULL;
  }

  ListHead->Flink = NextEntry;
  NextEntry->Blink = ListHead;

  return Entry;
}

FORCEINLINE BOOLEAN RemoveEntryList(_In_ PLIST_ENTRY Entry)
{
  PLIST_ENTRY PrevEntry;
  PLIST_ENTRY NextEntry;

  NextEntry = Entry->Flink;
  PrevEntry = Entry->Blink;
  if ((NextEntry->Blink != Entry) || (PrevEntry->Flink != Entry)) {
    printf("!!!FATAL LIST ERROR!!!\n");
    return FALSE;
  }

  PrevEntry->Flink = NextEntry;
  NextEntry->Blink = PrevEntry;
  return (BOOLEAN)(PrevEntry == NextEntry);
}
