#pragma once

#define DRIVER_PREFIX "KmdWorld:"

#define DbgPrintPrefix(s, ...) DbgPrint(DRIVER_PREFIX " " s "\n", __VA_ARGS__)

#define IOCTL_THREAD_UNHIDE_FROM_DEBUGGER \
  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2049, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define ADDPTR(ptr, val) ((unsigned char*)ptr + val)
#define SUBPTR(ptr, val) ((unsigned char*)ptr - val)

#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))
#define CLEAR_BIT(p, n) ((p) &= ~((1) << (n)))