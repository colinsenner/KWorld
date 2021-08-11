using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Security.Principal;

// TODO Automatically filter out system processes
// TODO Add checkbox to enable viewing of system processes

namespace KThreadUnhide
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        List<Process> processes = new List<Process>();

        public MainWindow()
        {
            InitializeComponent();

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
            var userProcesses = processes.Where(p => UserUtils.GetProcessUser(p) == UserUtils.GetCurrentUser()).ToList();

            // Order by start time
            //var userProcessesByStartTime = userProcesses.OrderByDescending(p => p.StartTime);

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
