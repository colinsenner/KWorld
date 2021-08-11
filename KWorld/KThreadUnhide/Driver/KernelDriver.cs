using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;
using System.IO;
using System.ComponentModel;
using System.Threading;

// https://www.codeproject.com/Articles/31905/A-C-class-wrapper-to-load-unload-device-drivers

namespace KThreadUnhide
{
    public class KernelDriver
    {
        private string serviceName;
        private string filePath;

        public KernelDriver(string serviceName, string filePath)
        {
            this.serviceName = serviceName;
            this.filePath = filePath;

            if (!File.Exists(filePath))
            {
                MessageBoxError($"Driver file not found: {filePath}.");
            }
        }

        private bool StartService()
        {
            var hManager = Services.OpenSCManager(null, null, SCM_ACCESS.SC_MANAGER_CONNECT);

            if (hManager == IntPtr.Zero)
                return false;

            // Try to open the service, if it's installed
            var hService = Services.OpenService(hManager, serviceName, SERVICE_ACCESS.SERVICE_START);

            if (hService != IntPtr.Zero)
            {
                // The service was started correctly
                Services.CloseServiceHandle(hService);
                return true;
            }

            int error = Marshal.GetLastWin32Error();

            Services.CloseServiceHandle(hManager);

            // If we get this specific error, that's fine, we'll create the service ourselves
            // Error 1060 - "The specified service does not exist as an installed service."
            if (error != 1060)
                throw new ApplicationException("Unexpected error while opening service");

            return false;
        }

        private bool CreateAndStartService()
        {
            var hManager = Services.OpenSCManager(null, null, SCM_ACCESS.SC_MANAGER_CREATE_SERVICE);

            if (hManager == IntPtr.Zero)
                return false;

            var hService = Services.CreateService(hManager,
                                              this.serviceName,
                                              this.serviceName,
                                              SERVICE_ACCESS.SERVICE_ALL_ACCESS,
                                              SERVICE_TYPES.SERVICE_KERNEL_DRIVER,
                                              SERVICE_START_TYPE.SERVICE_DEMAND_START,
                                              SERVICE_ERROR.SERVICE_ERROR_IGNORE,
                                              this.filePath,
                                              null,
                                              null,
                                              null,
                                              null,
                                              "");


            if (hService == IntPtr.Zero)
                return false;

            Services.CloseServiceHandle(hManager);

            // Start the service
            if (!Services.StartService(hService, 0, null))
            {
                MessageBoxErrorWithCode("Couldn't start the service");
                Services.CloseServiceHandle(hService);
                return false;
            }

            Services.CloseServiceHandle(hService);
            return true;
        }

        public bool Start()
        {
            if (!StartService())
            {
                // We weren't able to start the service, this is probably because it doesn't exist yet
                if (!CreateAndStartService())
                    return false;
            }

            return true;
        }

        // https://docs.microsoft.com/en-us/windows/win32/services/stopping-a-service
        public bool Stop()
        {
            var hManager = Services.OpenSCManager(null, null, SCM_ACCESS.SC_MANAGER_ALL_ACCESS);

            if (hManager == IntPtr.Zero)
            {
                MessageBoxErrorWithCode("OpenSCManager failed");
                return false;
            }

            var hService = Services.OpenService(hManager, serviceName, SERVICE_ACCESS.SERVICE_STOP);

            if (hService == IntPtr.Zero)
            {
                MessageBoxErrorWithCode("OpenService failed");
                Services.CloseServiceHandle(hManager);
                return false;
            }

            SERVICE_STATUS status = new SERVICE_STATUS();

            Services.ControlService(hService, SERVICE_CONTROL.STOP, ref status);

            var changedStatus = WaitForServiceStatus(hService, SERVICE_STATE.SERVICE_STOP_PENDING, SERVICE_STATE.SERVICE_STOPPED);

            if (!changedStatus)
                throw new ApplicationException("Unable to stop service");

            Services.CloseServiceHandle(hManager);

            if (!Services.DeleteService(hService))
            {
                MessageBoxErrorWithCode("DeleteService failed");
                Services.CloseServiceHandle(hService);
                return false;
            }

            Services.CloseServiceHandle(hService);

            return true;
        }

        private static bool WaitForServiceStatus(IntPtr service, SERVICE_STATE waitStatus, SERVICE_STATE desiredStatus)
        {
            SERVICE_STATUS status = new SERVICE_STATUS();

            Services.QueryServiceStatus(service, status);

            if (status.dwCurrentState == desiredStatus)
                return true;

            int dwStartTickCount = Environment.TickCount;
            int dwOldCheckPoint = status.dwCheckPoint;

            while (status.dwCurrentState == waitStatus)
            {
                // Do not wait longer than the wait hint. A good interval is
                // one tenth the wait hint, but no less than 1 second and no
                // more than 10 seconds.

                int dwWaitTime = status.dwWaitHint / 10;

                if (dwWaitTime < 1000) dwWaitTime = 1000;
                else if (dwWaitTime > 10000) dwWaitTime = 10000;

                Thread.Sleep(dwWaitTime);

                // Check the status again.

                if (Services.QueryServiceStatus(service, status) == 0) break;

                if (status.dwCheckPoint > dwOldCheckPoint)
                {
                    // The service is making progress.
                    dwStartTickCount = Environment.TickCount;
                    dwOldCheckPoint = status.dwCheckPoint;
                }
                else
                {
                    if (Environment.TickCount - dwStartTickCount > status.dwWaitHint)
                    {
                        // No progress made within the wait hint
                        break;
                    }
                }
            }
            return status.dwCurrentState == desiredStatus;
        }

        private void MessageBoxErrorWithCode(string message, int? errorCode = null)
        {
            errorCode = (errorCode == null) ? Marshal.GetLastWin32Error() : errorCode;

            MessageBox.Show($"{message} (code: {errorCode})\n\nMessage:\n{WinErrors.GetSystemMessage((int)errorCode)}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }

        private void MessageBoxError(string message)
        {
            MessageBox.Show($"{message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }
    }

    public static class Services
    {
        #region OpenSCManager
        [DllImport("advapi32.dll", EntryPoint = "OpenSCManagerW", ExactSpelling = true, CharSet = CharSet.Unicode, SetLastError = true)]
        public static extern IntPtr OpenSCManager(string machineName, string databaseName, SCM_ACCESS dwAccess);
        #endregion

        #region CreateService
        [DllImport("advapi32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern IntPtr CreateService(IntPtr hSCManager,
                                            string lpServiceName,
                                            string lpDisplayName,
                                            SERVICE_ACCESS dwDesiredAccess,
                                            SERVICE_TYPES dwServiceType,
                                            SERVICE_START_TYPE dwStartType,
                                            SERVICE_ERROR dwErrorControl,
                                            string lpBinaryPathName,
                                            string lpLoadOrderGroup,
                                            string lpdwTagId,
                                            string lpDependencies,
                                            string lpServiceStartName,
                                            string lpPassword);
        #endregion

        #region OpenService
        [DllImport("advapi32.dll", EntryPoint = "OpenServiceA", SetLastError = true, CharSet = CharSet.Ansi)]
        public static extern IntPtr OpenService(IntPtr hSCManager, string lpServiceName, SERVICE_ACCESS dwDesiredAccess);
        #endregion

        #region CloseServiceHandle
        [DllImport("advapi32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool CloseServiceHandle(IntPtr hSCObject);
        #endregion

        #region StartService
        [DllImport("advapi32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool StartService(
                IntPtr hService,
            int dwNumServiceArgs,
            string[] lpServiceArgVectors
            );
        #endregion

        #region ControlService
        [DllImport("advapi32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool ControlService(IntPtr hService, SERVICE_CONTROL dwControl, ref SERVICE_STATUS lpServiceStatus);
        #endregion

        #region QueryServiceStatus
        [DllImport("advapi32.dll")]
        public static extern int QueryServiceStatus(IntPtr hService, SERVICE_STATUS lpServiceStatus);
        #endregion

        #region DeleteService
        [DllImport("advapi32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool DeleteService(IntPtr hService);
        #endregion
    }
}
