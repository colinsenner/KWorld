#pragma once

#define DRIVER_PREFIX "KmdWorld:"
#define DRIVER_POOL_TAG 'WdmK'

#if DBG
// If compiled in debug mode print to DebugView
#define printk(s, ...) DbgPrint(DRIVER_PREFIX " " s "\n", __VA_ARGS__);
#else
#define printk(s, ...) DbgPrint(DRIVER_PREFIX " " s "\n", __VA_ARGS__);
#endif  // DBG

#define PtrAdd(ptr, offset) (PVOID)(((SIZE_T)ptr) + ((SIZE_T)offset))
#define PtrSub(ptr, offset) (PVOID)(((SIZE_T)ptr) - ((SIZE_T)offset))
#define PtrDeref(ptr, offset, type) *(type*)(PtrAdd(ptr, offset));

#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))
#define CLEAR_BIT(p, n) ((p) &= ~((1) << (n)))