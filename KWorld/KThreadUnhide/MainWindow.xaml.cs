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
        KernelDriver kmdWorld;

        public MainWindow()
        {
            InitializeComponent();

            ConsoleManager.Show();

            var driverPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "KmdWorld.sys");

            kmdWorld = new KernelDriver("KmdWorld", driverPath);

            Console.WriteLine("Creating KmdWorld service");
            if (!kmdWorld.Create())
            {
                MessageBox.Show("Error creating service", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
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


    }
}
