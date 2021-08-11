using System;
using System.Collections.Generic;
using System.Text;

namespace KThreadUnhide
{
    [Flags]
    public enum ACCESS_MASK : uint
    {
        DELETE = 0x00010000,
        READ_CONTROL = 0x00020000,
        WRITE_DAC = 0x00040000,
        WRITE_OWNER = 0x00080000,
        SYNCHRONIZE = 0x00100000,

        STANDARD_RIGHTS_REQUIRED = 0x000F0000,

        STANDARD_RIGHTS_READ = 0x00020000,
        STANDARD_RIGHTS_WRITE = 0x00020000,
        STANDARD_RIGHTS_EXECUTE = 0x00020000,

        STANDARD_RIGHTS_ALL = 0x001F0000,

        SPECIFIC_RIGHTS_ALL = 0x0000FFFF,

        ACCESS_SYSTEM_SECURITY = 0x01000000,

        MAXIMUM_ALLOWED = 0x02000000,

        GENERIC_READ = 0x80000000,
        GENERIC_WRITE = 0x40000000,
        GENERIC_EXECUTE = 0x20000000,
        GENERIC_ALL = 0x10000000,

        DESKTOP_READOBJECTS = 0x00000001,
        DESKTOP_CREATEWINDOW = 0x00000002,
        DESKTOP_CREATEMENU = 0x00000004,
        DESKTOP_HOOKCONTROL = 0x00000008,
        DESKTOP_JOURNALRECORD = 0x00000010,
        DESKTOP_JOURNALPLAYBACK = 0x00000020,
        DESKTOP_ENUMERATE = 0x00000040,
        DESKTOP_WRITEOBJECTS = 0x00000080,
        DESKTOP_SWITCHDESKTOP = 0x00000100,

        WINSTA_ENUMDESKTOPS = 0x00000001,
        WINSTA_READATTRIBUTES = 0x00000002,
        WINSTA_ACCESSCLIPBOARD = 0x00000004,
        WINSTA_CREATEDESKTOP = 0x00000008,
        WINSTA_WRITEATTRIBUTES = 0x00000010,
        WINSTA_ACCESSGLOBALATOMS = 0x00000020,
        WINSTA_EXITWINDOWS = 0x00000040,
        WINSTA_ENUMERATE = 0x00000100,
        WINSTA_READSCREEN = 0x00000200,

        WINSTA_ALL_ACCESS = 0x0000037F
    }

    public enum SERVICE_ERROR : uint
    {
        /// <summary>
        /// The startup program ignores the error and continues the startup
        /// operation.
        /// </summary>
        SERVICE_ERROR_IGNORE = 0x00000000,

        /// <summary>
        /// The startup program logs the error in the event log but continues
        /// the startup operation.
        /// </summary>
        SERVICE_ERROR_NORMAL = 0x00000001,

        /// <summary>
        /// The startup program logs the error in the event log. If the
        /// last-known-good configuration is being started, the startup
        /// operation continues. Otherwise, the system is restarted with
        /// the last-known-good configuration.
        /// </summary>
        SERVICE_ERROR_SEVERE = 0x00000002,

        /// <summary>
        /// The startup program logs the error in the event log, if possible.
        /// If the last-known-good configuration is being started, the startup
        /// operation fails. Otherwise, the system is restarted with the
        /// last-known good configuration.
        /// </summary>
        SERVICE_ERROR_CRITICAL = 0x00000003,
    }

    [Flags]
    public enum SERVICE_ACCESS : uint
    {
        STANDARD_RIGHTS_REQUIRED = 0xF0000,
        SERVICE_QUERY_CONFIG = 0x00001,
        SERVICE_CHANGE_CONFIG = 0x00002,
        SERVICE_QUERY_STATUS = 0x00004,
        SERVICE_ENUMERATE_DEPENDENTS = 0x00008,
        SERVICE_START = 0x00010,
        SERVICE_STOP = 0x00020,
        SERVICE_PAUSE_CONTINUE = 0x00040,
        SERVICE_INTERROGATE = 0x00080,
        SERVICE_USER_DEFINED_CONTROL = 0x00100,
        SERVICE_ALL_ACCESS = (STANDARD_RIGHTS_REQUIRED |
                          SERVICE_QUERY_CONFIG |
                          SERVICE_CHANGE_CONFIG |
                          SERVICE_QUERY_STATUS |
                          SERVICE_ENUMERATE_DEPENDENTS |
                          SERVICE_START |
                          SERVICE_STOP |
                          SERVICE_PAUSE_CONTINUE |
                          SERVICE_INTERROGATE |
                          SERVICE_USER_DEFINED_CONTROL)
    }

    public enum SERVICE_TYPE : uint
    {
        SERVICE_KERNEL_DRIVER = 0x00000001,
        SERVICE_FILE_SYSTEM_DRIVER = 0x00000002,
        SERVICE_WIN32_OWN_PROCESS = 0x00000010,
        SERVICE_WIN32_SHARE_PROCESS = 0x00000020,
        SERVICE_INTERACTIVE_PROCESS = 0x00000100
    }

    public enum SERVICE_START_TYPE : uint
    {
        SERVICE_AUTO_START = 0x00000002,
        SERVICE_BOOT_START = 0x00000000,
        SERVICE_DEMAND_START = 0x00000003,
        SERVICE_DISABLED = 0x00000004,
        SERVICE_SYSTEM_START = 0x00000001,

    }

    [Flags]
    public enum SCM_ACCESS : uint
    {
        SC_MANAGER_CONNECT = 0x0001,
        SC_MANAGER_CREATE_SERVICE = 0x0002,
        SC_MANAGER_ENUMERATE_SERVICE = 0x0004,
        SC_MANAGER_LOCK = 0x0008,
        SC_MANAGER_QUERY_LOCK_STATUS = 0x0010,
        SC_MANAGER_MODIFY_BOOT_CONFIG = 0x0020,

        SC_MANAGER_ALL_ACCESS = (ACCESS_MASK.STANDARD_RIGHTS_REQUIRED |
                                        SC_MANAGER_CONNECT |
                                        SC_MANAGER_CREATE_SERVICE     |
                                        SC_MANAGER_ENUMERATE_SERVICE  |
                                        SC_MANAGER_LOCK               |
                                        SC_MANAGER_QUERY_LOCK_STATUS  |
                                        SC_MANAGER_MODIFY_BOOT_CONFIG)
    }
}
