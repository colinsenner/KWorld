#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdio.h>
#include <stdlib.h>
#include "TrackProcesses.h"

int main() {
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  AddProcess((HANDLE)1);
  AddProcess((HANDLE)2);
  AddProcess((HANDLE)3);

  RemoveProcess((HANDLE)2);

  FreeTrackedProcesses();

  return 0;
}
