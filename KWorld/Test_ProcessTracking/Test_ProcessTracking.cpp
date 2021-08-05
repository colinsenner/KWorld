#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdio.h>
#include <stdlib.h>
#include "TrackProcesses.h"
#include <time.h>

#define RAND_RANGE(minimum, maximum) ((size_t)minimum + ((size_t)rand() % ((size_t)maximum - (size_t)minimum + 1)))

int main() {
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  srand((unsigned int)time(NULL));

  InitializeTrackedProcesses();

  for (size_t i = 0; i < 50; i++) {
    size_t pid = (size_t)RAND_RANGE(1, 50);

    bool addProcess = RAND_RANGE(1, 2) == 1;
    if (addProcess) {
      AddProcess((HANDLE)pid);
    } else {
      RemoveProcess((HANDLE)pid);
    }

    //bool addThread = RAND_RANGE(1, 2) == 1;
    //if (addThread) {
    //  for (size_t tid = 0; tid < RAND_RANGE(1, 9); tid++) {
    //    AddThreadToProcess((HANDLE)pid, (HANDLE)tid);
    //  }
    //} else {
    //  for (size_t tid = 0; tid < (size_t)RAND_RANGE(1, 9); tid++) {
    //    RemoveThreadFromProcess((HANDLE)pid, (HANDLE)tid);
    //  }
    //}
  }

  FreeTrackedProcesses();

  //auto non_existant_process = AddThreadToProcess((HANDLE)9, (HANDLE)901);

  //auto p1 = AddProcess((HANDLE)1);
  //auto th1 = AddThreadToProcess((HANDLE)1, (HANDLE)101);
  //auto th2 = AddThreadToProcess((HANDLE)1, (HANDLE)102);
  //auto th3 = AddThreadToProcess((HANDLE)1, (HANDLE)103);

  //printf("Is process 1 tracked: %d\n", IsProcessTracked((HANDLE)1));
  //printf("Is process 9 tracked: %d\n", IsProcessTracked((HANDLE)9));

  //PrintProcessList();

  //RemoveThreadFromProcess((HANDLE)1, (HANDLE)102);

  //PrintProcessList();



  //PrintProcessList();

  return 0;
}
