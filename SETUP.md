# KWorld
Kernel driver for removing ThreadHideFromDebugger from processes.

# Installations
* Turn Windows Features on of off: Hyper-V
* VMWare Player

# Setup VMWare

## BCDEdit

```bcdedit /debug on```

## KDNET
* https://www.youtube.com/watch?v=V7DJ_ptkOpM

### On host machine
```
cmd> ipconfig

...
Ethernet adapter VMware Network Adapter VMnet1:

   Connection-specific DNS Suffix  . :
   Link-local IPv6 Address . . . . . : fe80::fdd0:db3a:2bfc:db59%23
   Autoconfiguration IPv4 Address. . : 169.254.219.89                   <<<<<<< this ip address
   Subnet Mask . . . . . . . . . . . : 255.255.0.0
   Default Gateway . . . . . . . . . :
...

```

### On VM
```
cmd> kdnet <host_ip> <port>
```

## Setup Host machine
* Windows SDK https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk/

## Batch scripts to setup your environment are located in
```/KWorld/scripts```

## DebugView
If you're not seeing output from DebugView make sure to enable the registry settings "Debug Print Filter" (.reg file in /scripts)