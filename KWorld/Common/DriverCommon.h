#pragma once

#define DRIVER_PREFIX "KmdWorld:"
#define DRIVER_POOL_TAG 'WdmK'

#define DbgPrintPrefix(s, ...)                          \
  DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_TRACE_LEVEL, \
             DRIVER_PREFIX " " s "\n", __VA_ARGS__)

#define IOCTL_THREAD_UNHIDE_FROM_DEBUGGER \
  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2049, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PtrAdd(ptr, offset) (PVOID)(((SIZE_T)ptr) + ((SIZE_T)offset))
#define PtrSub(ptr, offset) (PVOID)(((SIZE_T)ptr) - ((SIZE_T)offset))
#define PtrDeref(ptr, offset, type) *(type*)(PtrAdd(ptr, offset));

#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))
#define CLEAR_BIT(p, n) ((p) &= ~((1) << (n)))