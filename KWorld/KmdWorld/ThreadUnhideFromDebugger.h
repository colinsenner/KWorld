#pragma once
#include <ntifs.h>
#include <ntddk.h>

#include "..\Common\DriverCommon.h"

NTSTATUS ThreadUnhideFromDebugger(ProcessData data);