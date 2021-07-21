#pragma once

#define DRIVER_PREFIX "KmdWorld:"
#define DRIVER_POOL_TAG 'WdmK'

#define DbgPrintPrefix(s, ...) DbgPrint(DRIVER_PREFIX " " s "\n", __VA_ARGS__)