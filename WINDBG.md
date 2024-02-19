# WinDbg commands

Helpful commands for debugging Windows kernel and user mode applications.

```bash
# List this process
!process 0 0 NoBreakpointsAllowed.exe

# List the process threads
!process ffff8008b57c2080 2

# List thread information for a specific thread
!thread ffffe00000e6d080

# List the _EPROCESS structure
dt nt!_EPROCESS addr

# List the _ETHREAD structure
dt nt!_ETHREAD 0xffffa103f083f568

# CrossThreadFlags
db 0xffffa103f083f568+560

# ThreadListHead
dt _LIST_ENTRY ffffa103f3796080+5e0

db ffffa103f083f080+510

ffffa103`f083f590  06 54 00 00 00 00 00 00-00 00 00 00 00 00 00 00  .T..............
ffffa103`f083f5a0  00 00 00 00 00 00 00 00-00 00 00 00 00 00 00 00  ................
ffffa103`f083f5b0  00 00 00 00 00 00 00 00-00 00 00 00 00 00 00 00  ................
ffffa103`f083f5c0  00 00 00 00 00 00 00 00-00 00 00 00 00 00 00 00  ................
ffffa103`f083f5d0  00 00 00 00 00 00 00 00-00 00 00 00 01 00 00 00  ................
ffffa103`f083f5e0  e0 f5 83 f0 03 a1 ff ff-e0 f5 83 f0 03 a1 ff ff  ................
ffffa103`f083f5f0  f0 f5 83 f0 03 a1 ff ff-f0 f5 83 f0 03 a1 ff ff  ................
ffffa103`f083f600  00 00 00 00 00 00 00 00-00 00 00 00 00 00 00 00  ................

# Unset bit 3 (0x4)
eb ffffa103`f083f590 2  (6, unset bit 3 = 2)
```

# Structures
```bash
# _EPROCESS
+0x5e0 ThreadListHead   : _LIST_ENTRY

# _ETHREAD
+0x4e8 ThreadListEntry  : _LIST_ENTRY
```