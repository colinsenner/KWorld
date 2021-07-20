# KWorld
Kernel driver for removing ThreadHideFromDebugger from processes.

# Installations
* Turn Windows Features on of off: Hyper-V
* VMWare Player

# Setup VMWare

## BCDEdit

```bcdedit /debug on```

```bcdedit /dbgsettings serial debugport:n baudrate:115200```

where n is the number of a COM port on the virtual machine.

## KDNET
* https://www.youtube.com/watch?v=V7DJ_ptkOpM

## Create a named pipe
* https://docs.vmware.com/en/VMware-Workstation-Pro/16.0/com.vmware.ws.using.doc/GUID-70C25BED-6791-4AF2-B530-8030E39ED749.html

```\\.\pipe\kdebug```

# Setup Host machine
* Windows SDK https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk/

