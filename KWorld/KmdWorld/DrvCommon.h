#pragma once

#define DRIVER_PREFIX "KmdWorld:"

#define DbgPrintPrefix(s, ...) DbgPrint(DRIVER_PREFIX " " s "\n", __VA_ARGS__)