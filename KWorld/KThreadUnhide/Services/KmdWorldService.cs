using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace KThreadUnhide
{
    public static class KmdWorldService
    {
        [StructLayout(LayoutKind.Sequential)]
        private class SERVICE_STATUS
        {
            public int dwServiceType = 0;
            public ServiceState dwCurrentState = 0;
            public int dwControlsAccepted = 0;
            public int dwWin32ExitCode = 0;
            public int dwServiceSpecificExitCode = 0;
            public int dwCheckPoint = 0;
            public int dwWaitHint = 0;
        }

        #region OpenSCManager
        [DllImport("advapi32.dll", EntryPoint = "OpenSCManagerW", ExactSpelling = true, CharSet = CharSet.Unicode, SetLastError = true)]
        static extern IntPtr OpenSCManager(string machineName, string databaseName, ScmAccessRights dwDesiredAccess);
        #endregion

        #region OpenService
        [DllImport("advapi32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        static extern IntPtr OpenService(IntPtr hSCManager, string lpServiceName, ServiceAccessRights dwDesiredAccess);
        #endregion

        #region CreateService
        [DllImport("advapi32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        private static extern IntPtr CreateService(IntPtr hSCManager, string lpServiceName, string lpDisplayName, ServiceAccessRights dwDesiredAccess, ServiceType dwServiceType, ServiceBootFlag dwStartType, ServiceError dwErrorControl, string lpBinaryPathName, string lpLoadOrderGroup, IntPtr lpdwTagId, string lpDependencies, string lp, string lpPassword);
        #endregion

        #region CloseServiceHandle
        [DllImport("advapi32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool CloseServiceHandle(IntPtr hSCObject);
        #endregion

        #region QueryServiceStatus
        [DllImport("advapi32.dll")]
        private static extern int QueryServiceStatus(IntPtr hService, SERVICE_STATUS lpServiceStatus);
        #endregion

        #region DeleteService
        [DllImport("advapi32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool DeleteService(IntPtr hService);
        #endregion

        #region ControlService
        [DllImport("advapi32.dll")]
        private static extern int ControlService(IntPtr hService, ServiceControl dwControl, SERVICE_STATUS lpServiceStatus);
        #endregion

        #region StartService
        [DllImport("advapi32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool StartService(IntPtr hService, int dwNumServiceArgs, string[] lpServiceArgVectors);
        #endregion

        public static void Delete(string serviceName)
        {
            var scm = OpenSCManagerWrapper(ScmAccessRights.AllAccess);
            var service = OpenServiceWrapper(scm, serviceName, ServiceAccessRights.AllAccess);

            try
            {
                if (!DeleteService(service))
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
            }
            finally
            {
                CloseServiceHandle(service);
                CloseServiceHandle(scm);
            }
        }

        public static bool IsInstalled(string serviceName)
        {
            var scm = OpenSCManagerWrapper(ScmAccessRights.AllAccess);
            var service = OpenServiceWrapper(scm, serviceName, ServiceAccessRights.AllAccess);
            bool installed = false;

            if (service != IntPtr.Zero)
            {
                installed = true;
            }
            else
            {
                int error = Marshal.GetLastWin32Error();

                if (error != (int)WinError.ERROR_SERVICE_DOES_NOT_EXIST)
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
            }

            CloseServiceHandle(service);
            CloseServiceHandle(scm);

            return installed;
        }

        public static void Install(string serviceName, string displayName, string fileName)
        {
            IntPtr service = IntPtr.Zero;
            var scm = OpenSCManagerWrapper(ScmAccessRights.CreateService);

            try
            {
                service = CreateService(scm,
                                            serviceName,
                                            displayName,
                                            ServiceAccessRights.AllAccess,
                                            ServiceType.KernelDriver,
                                            ServiceBootFlag.DemandStart,
                                            ServiceError.Normal,
                                            fileName,
                                            null,
                                            IntPtr.Zero,
                                            null,
                                            null,
                                            "");

                if (service == IntPtr.Zero)
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
            }
            finally
            {
                CloseServiceHandle(scm);
                CloseServiceHandle(service);
            }
        }

        public static void Start(string serviceName)
        {
            var scm = OpenSCManagerWrapper(ScmAccessRights.Connect);
            var service = OpenServiceWrapper(scm, serviceName, ServiceAccessRights.QueryStatus | ServiceAccessRights.Start);

            try
            {
                StartServiceWrapper(service);
            }
            finally
            {
                CloseServiceHandle(service);
                CloseServiceHandle(scm);
            }
        }


        private static bool WaitForServiceStatus(IntPtr service, ServiceState waitState, ServiceState desiredState)
        {
            var status = GetServiceStatusWrapper(service);

            if (status.dwCurrentState == desiredState)
                return true;

            int dwStartTime = Environment.TickCount;
            int dwTimeout = 30000; // 30-second time-out

            while (status.dwCurrentState == waitState)
            {
                // Do not wait longer than the wait hint. A good interval is
                // one-tenth of the wait hint but not less than 1 second
                // and not more than 10 seconds.

                int dwWaitTime = status.dwWaitHint / 10;

                if (dwWaitTime < 1000)
                    dwWaitTime = 1000;
                else if (dwWaitTime > 10000)
                    dwWaitTime = 10000;

                Thread.Sleep(dwWaitTime);

                // We've slept to allow the driver to stop itself
                // Check the status again, if we haven't stopped
                status = GetServiceStatusWrapper(service);

                if (status.dwCurrentState == desiredState)
                    break;

                if (Environment.TickCount - dwStartTime > dwTimeout)
                    throw new ApplicationException($"Timed out waiting for driver to reach {desiredState}");
            }

            return status.dwCurrentState == desiredState;
        }

        public static void Stop(string serviceName)
        {
            var scm = OpenSCManagerWrapper(ScmAccessRights.AllAccess);
            var service = OpenServiceWrapper(scm, serviceName, ServiceAccessRights.QueryStatus | ServiceAccessRights.Stop);

            try
            {
                StopServiceWrapper(service);
            }
            finally
            {
                CloseServiceHandle(service);
                CloseServiceHandle(scm);
            }

        }

        private static ServiceState GetServiceState(IntPtr service)
        {
            var status = GetServiceStatusWrapper(service);

            return status.dwCurrentState;
        }

        private static SERVICE_STATUS GetServiceStatusWrapper(IntPtr service)
        {
            SERVICE_STATUS status = new SERVICE_STATUS();

            if (QueryServiceStatus(service, status) == 0)
            {
                throw new Win32Exception(Marshal.GetLastWin32Error());
            }

            return status;
        }

        private static void StartServiceWrapper(IntPtr service)
        {
            // Check if it's already running
            var state = GetServiceState(service);
            if (state != ServiceState.Stopped && state != ServiceState.StopPending)
                return;

            // Start the service
            if (!StartService(service, 0, null))
            {
                throw new Win32Exception(Marshal.GetLastWin32Error());
            }
        }

        private static void StopServiceWrapper(IntPtr service)
        {
            // Check if it's already stopped
            if (GetServiceState(service) == ServiceState.Stopped)
                return;

            // If a stop is pending, wait for it.
            if (WaitForServiceStatus(service, ServiceState.StopPending, ServiceState.Stopped))
            {
                return;
            }

            // Issue the stop command
            SERVICE_STATUS status = new SERVICE_STATUS();
            if (ControlService(service, ServiceControl.Stop, status) == 0)
            {
                throw new Win32Exception(Marshal.GetLastWin32Error());
            }

            //
            // Wait for the service to stop (with a timeout)
            //
            int dwStartTime = Environment.TickCount;
            int dwTimeout = 30000; // 30 second time-out

            while (status.dwCurrentState != ServiceState.Stopped)
            {
                int dwWaitTime = status.dwWaitHint / 10;

                if (dwWaitTime < 1000)
                    dwWaitTime = 1000;
                else if (dwWaitTime > 10000)
                    dwWaitTime = 10000;

                Thread.Sleep(dwWaitTime);

                // We've slept to allow the driver to stop itself
                // Check the status again, if we haven't stopped
                status = GetServiceStatusWrapper(service);

                if (status.dwCurrentState == ServiceState.Stopped)
                    break;

                if (Environment.TickCount - dwStartTime > dwTimeout)
                    throw new ApplicationException($"Timed out waiting for driver to stop.");
            }

            // Service stopped successfully
        }

        private static IntPtr OpenSCManagerWrapper(ScmAccessRights rights)
        {
            var scm = OpenSCManager(null, null, rights);
            if (scm == IntPtr.Zero)
            {
                throw new Win32Exception(Marshal.GetLastWin32Error());
            }

            return scm;
        }

        private static IntPtr OpenServiceWrapper(IntPtr scm, string serviceName, ServiceAccessRights rights)
        {
            var service = OpenService(scm, serviceName, rights);
            if (service == IntPtr.Zero)
            {
                throw new Win32Exception(Marshal.GetLastWin32Error());
            }

            return service;
        }

        #region Service enumerations

        public enum ServiceState
        {
            Unknown = -1, // The state cannot be (has not been) retrieved.
            NotFound = 0, // The service is not known on the host server.
            Stopped = 1,
            StartPending = 2,
            StopPending = 3,
            Running = 4,
            ContinuePending = 5,
            PausePending = 6,
            Paused = 7
        }

        public enum ServiceType : uint
        {
            KernelDriver = 0x00000001,
            SystemDriver = 0x00000002,
            Win32OwnProcess = 0x00000010,
            Win32ShareProcess = 0x00000020,
            InteractiveProcess = 0x00000100
        }

        [Flags]
        public enum ScmAccessRights
        {
            Connect = 0x0001,
            CreateService = 0x0002,
            EnumerateService = 0x0004,
            Lock = 0x0008,
            QueryLockStatus = 0x0010,
            ModifyBootConfig = 0x0020,
            StandardRightsRequired = 0xF0000,
            AllAccess = (StandardRightsRequired | Connect | CreateService |
                         EnumerateService | Lock | QueryLockStatus | ModifyBootConfig)
        }

        [Flags]
        public enum ServiceAccessRights
        {
            QueryConfig = 0x1,
            ChangeConfig = 0x2,
            QueryStatus = 0x4,
            EnumerateDependants = 0x8,
            Start = 0x10,
            Stop = 0x20,
            PauseContinue = 0x40,
            Interrogate = 0x80,
            UserDefinedControl = 0x100,
            Delete = 0x00010000,
            StandardRightsRequired = 0xF0000,
            AllAccess = (StandardRightsRequired | QueryConfig | ChangeConfig |
                         QueryStatus | EnumerateDependants | Start | Stop | PauseContinue |
                         Interrogate | UserDefinedControl)
        }

        public enum ServiceBootFlag
        {
            Start = 0x00000000,
            SystemStart = 0x00000001,
            AutoStart = 0x00000002,
            DemandStart = 0x00000003,
            Disabled = 0x00000004
        }

        public enum ServiceControl
        {
            Stop = 0x00000001,
            Pause = 0x00000002,
            Continue = 0x00000003,
            Interrogate = 0x00000004,
            Shutdown = 0x00000005,
            ParamChange = 0x00000006,
            NetBindAdd = 0x00000007,
            NetBindRemove = 0x00000008,
            NetBindEnable = 0x00000009,
            NetBindDisable = 0x0000000A
        }

        public enum ServiceError
        {
            Ignore = 0x00000000,
            Normal = 0x00000001,
            Severe = 0x00000002,
            Critical = 0x00000003
        }

        public enum WinError : int
        {
            ERROR_SERVICE_DOES_NOT_EXIST = 1060,
        }
        #endregion
    }
}
