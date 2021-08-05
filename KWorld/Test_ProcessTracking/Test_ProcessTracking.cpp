#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdio.h>
#include <stdlib.h>
#include "TrackProcesses.h"

int main() {
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  auto proc1 = AddProcess((HANDLE)1);
  auto proc2 = AddProcess((HANDLE)2);
  auto proc3 = AddProcess((HANDLE)3);

  RemoveProcess(proc2);

  FreeTrackedProcesses();

  return 0;
}
