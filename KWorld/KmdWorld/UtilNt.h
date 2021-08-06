#pragma once
#include <ntddk.h>
#include "undocumented.h"

NTSTATUS GetSystemProcessInfo(HANDLE pid, _Out_ PSYSTEM_PROCESS_INFORMATION SystemProcessInfo);