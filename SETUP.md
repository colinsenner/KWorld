# VMWare Setup for Kernel Debugging

## Steps

* Install Windows 11 Pro on VMWare
* Set up kernel debugging [here](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/setting-up-kernel-mode-debugging-in-windbg--cdb--or-ntsd)
* Copy over the compiled binaries onto the target

## On the target

```bash
# Elevated prompt
bcdedit /debug on
bcdedit /set testsigning on
```

## Manually controlling the kernel driver

The `KThreadUnhide` GUI program automatically starts/stops/removes the `KmdWorld` driver automatically. Should you run into issues you can do it manually.

```bash
# Copy KmdWorld.sys to C:\Windows\System32\drivers

# Create the service
sc create KmdWorld type= kernel binpath= "C:\Windows\System32\Drivers\KmdWorld.sys"

# Start the service
sc start KmdWorld

# Stop the service
sc stop KmdWorld

# Remove the service
sc delete KmdWorld
```

## DebugView
If you're not seeing output from DebugView make sure to enable the registry settings "Debug Print Filter" (.reg file in /scripts)

## References
* https://www.youtube.com/watch?v=V7DJ_ptkOpM
