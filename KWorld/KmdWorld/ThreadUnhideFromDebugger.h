#pragma once
#include <ntifs.h>
#include <ntddk.h>

NTSTATUS ThreadUnhideFromDebugger(HANDLE pid);