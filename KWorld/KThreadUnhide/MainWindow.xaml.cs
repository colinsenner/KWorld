using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Diagnostics;

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

        public void PopulateProcesses()
        {
            processDataGrid.ItemsSource = null;
            processDataGrid.Items.Clear();

            processDataGrid.ItemsSource = Process.GetProcesses();
        }

        private void refreshButton_Click(object sender, RoutedEventArgs e)
        {
            PopulateProcesses();
        }

        private void unhideButton_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}
