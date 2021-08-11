using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.IO;

// TODO Automatically filter out system processes
// TODO Add checkbox to enable viewing of system processes

namespace KThreadUnhide
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        KernelDriver KmdWorldDriver;

        public MainWindow()
        {
            InitializeComponent();

            ConsoleManager.Show();

            var driverPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "KmdWorld.sys");

            KmdWorldDriver = new KernelDriver("KmdWorld", driverPath);

            Console.WriteLine("Creating KmdWorld service");
            if (!KmdWorldDriver.Start())
            {
                MessageBox.Show("Couldn't start driver service.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                Application.Current.Shutdown();
            }

            PopulateProcesses();
        }

        public void ClearDataList()
        {
            processDataGrid.ItemsSource = null;
            processDataGrid.Items.Clear();
        }

        public void PopulateProcesses()
        {
            if ((bool)showAllProcesses.IsChecked)
                PopulateAllProcesses();
            else
                PopulateUserProcesses();
        }

        private void PopulateAllProcesses()
        {
            ClearDataList();
            processDataGrid.ItemsSource = Process.GetProcesses();
        }

        private void PopulateUserProcesses()
        {
            ClearDataList();

            var processes = Process.GetProcesses();
            var userProcesses = processes.Where(p => UserUtils.GetProcessUser(p) == UserUtils.GetCurrentUser());

            processDataGrid.ItemsSource = userProcesses;
        }

        private void refreshButton_Click(object sender, RoutedEventArgs e)
        {
            PopulateProcesses();
        }

        private void showAllProcesses_Changed(object sender, RoutedEventArgs e)
        {
            PopulateProcesses();
        }

        private void unhideButton_Click(object sender, RoutedEventArgs e)
        {

        }

        private void Window_Closed(object sender, EventArgs e)
        {
            KmdWorldDriver.Stop();
        }
    }
}
