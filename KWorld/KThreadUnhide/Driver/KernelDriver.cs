using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;
using System.IO;

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

        private bool TryStartService()
        {
            var hManager = GetManager(SCM_ACCESS.SC_MANAGER_CONNECT);

            if (hManager == IntPtr.Zero)
                return false;

            //
            // Try to open the service, if it's installed
            //
            var hService = GetService(;

            if (hService != IntPtr.Zero)
            {
                // The service was started correctly
                Services.CloseServiceHandle(hManager);
                Services.CloseServiceHandle(hService);
                return true;
            }

            int error = Marshal.GetLastWin32Error();

            // If we get this specific error, that's fine, we'll create the service ourselves
            // Error 1060 - "The specified service does not exist as an installed service."
            if (error != 1060)
                throw new ApplicationException("Unexpected error while opening service");

            Services.CloseServiceHandle(hManager);
            Services.CloseServiceHandle(hService);

            return false;
        }

        private IntPtr GetManager(SCM_ACCESS access = SCM_ACCESS.SC_MANAGER_ALL_ACCESS)
        {
            var hManager = Services.OpenSCManager(null, null, access);

            if (hManager == null)
                return IntPtr.Zero;

            return hManager;
        }

        private IntPtr GetService(SCM_ACCESS scm_access, SERVICE_ACCESS service_access)
        {
            var hManager = GetManager(scm_access);

            var hService = Services.OpenService(hManager, serviceName, service_access);

            Services.CloseServiceHandle(hManager);

            return hService;
        }

        private bool CreateService()
        {
            var hManager = GetManager();

            if (hManager == null)
                return false;

            var hService = Services.CreateService(hManager,
                                              serviceName,
                                              serviceName,
                                              SERVICE_ACCESS.SERVICE_ALL_ACCESS,
                                              SERVICE_TYPE.SERVICE_KERNEL_DRIVER,
                                              SERVICE_START_TYPE.SERVICE_DEMAND_START,
                                              SERVICE_ERROR.SERVICE_ERROR_IGNORE,
                                              filePath,
                                              null,
                                              null,
                                              null,
                                              null,
                                              "");

            if (!Services.StartService(hService, 0, null))
                return false;

            Services.CloseServiceHandle(hManager);
            Services.CloseServiceHandle(hService);

            return true;
        }

        public bool DeleteService()
        {
            var hManager = Services.OpenSCManager(null, null, SCM_ACCESS.SC_MANAGER_ALL_ACCESS);

            if (hManager == null)
                return false;



            return true;
        }

        public bool Start()
        {
            if (!TryStartService())
            {
                // We weren't able to start the service, this is probably because it doesn't exist yet
                if (!CreateService())
                {
                    MessageBoxErrorWithCode("Couldn't create service");
                    return false;
                }
            }

            return true;
        }

        public bool Stop()
        {
            return true;
        }

        private void MessageBoxErrorWithCode(string message)
        {
            int error = Marshal.GetLastWin32Error();

            MessageBox.Show($"{message} (code: {error})", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }

        private void MessageBoxError(string message)
        {
            MessageBox.Show($"{message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }
    }

    public static class Services
    {
        [DllImport("advapi32.dll", EntryPoint = "OpenSCManagerW", ExactSpelling = true, CharSet = CharSet.Unicode, SetLastError = true)]
        public static extern IntPtr OpenSCManager(string machineName, string databaseName, SCM_ACCESS dwAccess);

        [DllImport("advapi32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern IntPtr CreateService(IntPtr hSCManager,
                                            string lpServiceName,
                                            string lpDisplayName,
                                            SERVICE_ACCESS dwDesiredAccess,
                                            SERVICE_TYPE dwServiceType,
                                            SERVICE_START_TYPE dwStartType,
                                            SERVICE_ERROR dwErrorControl,
                                            string lpBinaryPathName,
                                            string lpLoadOrderGroup,
                                            string lpdwTagId,
                                            string lpDependencies,
                                            string lpServiceStartName,
                                            string lpPassword);

        [DllImport("advapi32.dll", EntryPoint = "OpenServiceA", SetLastError = true, CharSet = CharSet.Ansi)]
        public static extern IntPtr OpenService(IntPtr hSCManager, string lpServiceName, SERVICE_ACCESS dwDesiredAccess);

        [DllImport("advapi32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool CloseServiceHandle(IntPtr hSCObject);

        [DllImport("advapi32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool StartService(
                IntPtr hService,
            int dwNumServiceArgs,
            string[] lpServiceArgVectors
            );
    }
}
