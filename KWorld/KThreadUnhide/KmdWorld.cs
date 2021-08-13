using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;

namespace KThreadUnhide
{
    public class KmdWorld
    {
        IntPtr _device;

        public enum ControlCode : uint
        {
            IOCTL_THREAD_UNHIDE_FROM_DEBUGGER = 0x228124
        }

        public KmdWorld()
        {
            OpenDriverHandle();
        }

        ~KmdWorld()
        {
            CloseDriverHandle();
        }

        private void OpenDriverHandle()
        {
            _device = CreateFile("\\\\.\\KmdWorld", FileAccessModes.GENERIC_ALL, 0, IntPtr.Zero, CreationDisposition.OPEN_EXISTING, FlagsAndAttributes.FILE_ATTRIBUTE_SYSTEM, IntPtr.Zero);

            if (_device == IntPtr.Zero)
            {
                var error = Marshal.GetLastWin32Error();
                throw new ApplicationException($"Couldn't get a handle to the driver (code {error}).");
            }
        }

        private void CloseDriverHandle()
        {
            CloseHandle(_device);
        }

        #region Custom C++ structs we pass to the kernel
        [StructLayout(LayoutKind.Sequential)]
        struct ProcessData
        {
            public int ProcessId;
        };
        #endregion

        public void ThreadUnhideFromDebugger(int pid)
        {
            bool status;
            IntPtr pData = IntPtr.Zero;

            // This is the struct we pass from C++
            var data = new ProcessData() {
                ProcessId = pid
            };

            try
            {
                pData = Marshal.AllocHGlobal(Marshal.SizeOf(data));

                Marshal.StructureToPtr(data, pData, false);

                uint _unused = 0;
                status = DeviceIoControl(_device, (uint)ControlCode.IOCTL_THREAD_UNHIDE_FROM_DEBUGGER, pData, (uint)Marshal.SizeOf<ProcessData>(), IntPtr.Zero, 0, out _unused, IntPtr.Zero);
            }
            finally
            {
                Marshal.FreeHGlobal(pData);
            }


            if (status)
            {
                MessageBox.Show("Unhid all threads from debugger.", "Success");
            }
            else
            {
                throw new ApplicationException($"DeviceIoControl error (code {Marshal.GetLastWin32Error()})");
            }
        }

        #region DeviceIoControl
        [DllImport("kernel32.dll", ExactSpelling = true, SetLastError = true, CharSet = CharSet.Auto)]
        private static extern bool DeviceIoControl(IntPtr hDevice, uint dwIoControlCode,
                                            IntPtr lpInBuffer, uint nInBufferSize,
                                            IntPtr lpOutBuffer, uint nOutBufferSize,
                                            out uint lpBytesReturned, IntPtr lpOverlapped);
        #endregion

        #region CreateFile
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        private static extern IntPtr CreateFile(string lpFileName,
                                                FileAccessModes dwDesiredAccess,
                                                ShareMode dwShareMode,
                                                IntPtr SecurityAttributes,
                                                CreationDisposition dwCreationDisposition,
                                                FlagsAndAttributes dwFlagsAndAttributes,
                                                IntPtr hTemplateFile);
        #endregion

        #region CloseHandle
        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool CloseHandle(IntPtr hObject);
        #endregion

        #region File Access Enums
        public enum FileAccessModes : uint
        {
            GENERIC_READ = 0x80000000,
            GENERIC_WRITE = 0x40000000,
            GENERIC_EXECUTE = 0x20000000,
            GENERIC_ALL = 0x10000000
        }

        public enum CreationDisposition : int
        {
            CREATE_NEW = 1,
            CREATE_ALWAYS = 2,
            OPEN_EXISTING = 3,
            OPEN_ALWAYS = 4,
            TRUNCATE_EXISTING = 5
        }

        public enum FlagsAndAttributes : uint
        {
            FILE_ATTRIBUTE_READONLY = 0x00000001,
            FILE_ATTRIBUTE_HIDDEN = 0x00000002,
            FILE_ATTRIBUTE_SYSTEM = 0x00000004,
            FILE_ATTRIBUTE_DIRECTORY = 0x00000010,
            FILE_ATTRIBUTE_ARCHIVE = 0x00000020,
            FILE_ATTRIBUTE_DEVICE = 0x00000040,
            FILE_ATTRIBUTE_NORMAL = 0x00000080,
            FILE_ATTRIBUTE_TEMPORARY = 0x00000100,
            FILE_ATTRIBUTE_SPARSE_FILE = 0x00000200,
            FILE_ATTRIBUTE_REPARSE_POINT = 0x00000400,
            FILE_ATTRIBUTE_COMPRESSED = 0x00000800,
            FILE_ATTRIBUTE_OFFLINE = 0x00001000,
            FILE_ATTRIBUTE_NOT_CONTENT_INDEXED = 0x00002000,
            FILE_ATTRIBUTE_ENCRYPTED = 0x00004000,
            FILE_ATTRIBUTE_INTEGRITY_STREAM = 0x00008000,
            FILE_ATTRIBUTE_VIRTUAL = 0x00010000,
            FILE_ATTRIBUTE_NO_SCRUB_DATA = 0x00020000,
            FILE_ATTRIBUTE_EA = 0x00040000,
            FILE_ATTRIBUTE_PINNED = 0x00080000,
            FILE_ATTRIBUTE_UNPINNED = 0x00100000,
            FILE_ATTRIBUTE_RECALL_ON_OPEN = 0x00040000,
            FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS = 0x00400000,
        }

        public enum ShareMode : uint
        {
            FILE_SHARE_NONE = 0x00000000,
            FILE_SHARE_READ = 0x00000001,
            FILE_SHARE_WRITE = 0x00000002,
            FILE_SHARE_DELETE = 0x00000004,
        }
        #endregion
    }
}
