#pragma once
#include <ntddk.h>

namespace nt_undocumented {

  typedef enum _SYSTEM_INFORMATION_CLASS { SystemProcessesAndThreadsInformation = 0x5 } SYSTEM_INFORMATION_CLASS;

  // https://processhacker.sourceforge.io/doc/ntexapi_8h_source.html

  typedef unsigned char BYTE;

  typedef struct _SYSTEM_THREAD_INFORMATION {
    LARGE_INTEGER Reserved1[3];
    ULONG Reserved2;
    PVOID StartAddress;
    CLIENT_ID ClientId;
    KPRIORITY Priority;
    LONG BasePriority;
    ULONG Reserved3;
    ULONG ThreadState;
    ULONG WaitReason;
  } SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;

  typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    BYTE Reserved1[48];
    UNICODE_STRING ImageName;
    KPRIORITY BasePriority;
    HANDLE UniqueProcessId;
    PVOID Reserved2;
    ULONG HandleCount;
    ULONG SessionId;
    PVOID Reserved3;
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG Reserved4;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    PVOID Reserved5;
    SIZE_T QuotaPagedPoolUsage;
    PVOID Reserved6;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER Reserved7[6];
    SYSTEM_THREAD_INFORMATION Threads[1];
  } SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

  typedef NTSTATUS(NTAPI * fpZwQuerySystemInformation)(IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
                                                       OUT PVOID SystemInformation, IN ULONG SystemInformationLength,
                                                       OUT PULONG ReturnLength OPTIONAL);

  extern fpZwQuerySystemInformation _ZwQuerySystemInformation;

  NTSTATUS Init();

}  // namespace undocumented