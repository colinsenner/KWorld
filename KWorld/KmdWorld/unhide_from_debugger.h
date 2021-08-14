#pragma once
#include <ntifs.h>

#include "..\Common\ku_shared.h"

namespace kmdworld {

NTSTATUS ThreadUnhideFromDebugger(ProcessData data, ProcessDataComplete* outData);

}