#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdio.h>
#include <stdlib.h>
#include "TrackProcesses.h"

int main() {
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  auto non_existant_process = AddThreadToProcess((HANDLE)9, (HANDLE)901);

  auto p1 = AddProcess((HANDLE)1);
  auto th1 = AddThreadToProcess((HANDLE)1, (HANDLE)101);
  auto th2 = AddThreadToProcess((HANDLE)1, (HANDLE)102);
  auto th3 = AddThreadToProcess((HANDLE)1, (HANDLE)103);

  PrintProcessList();

  RemoveThreadFromProcess((HANDLE)1, (HANDLE)102);

  PrintProcessList();

  FreeTrackedProcesses();

  PrintProcessList();

  return 0;
}
