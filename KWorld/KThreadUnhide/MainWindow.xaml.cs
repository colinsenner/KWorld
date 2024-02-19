using System;
using System.Linq;
using System.Windows;
using System.Diagnostics;
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
        string driverName = "KmdWorld";
        KmdWorld driver;

        public MainWindow()
        {
            InitializeComponent();

            var driverPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, $"{driverName}.sys");

            if (!File.Exists(driverPath))
            {
                MessageBox.Show($"Driver doesn't exist\n{driverPath}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                Application.Current.Shutdown();
            }

            try
            {
                KmdWorldService.Install(driverName, driverName, driverPath);
                KmdWorldService.Start(driverName);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message, "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                Application.Current.Shutdown();
            }

            driver = new KmdWorld();

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
            DoUnhide();
        }

        private void OnMouseDoubleClicked(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            DoUnhide();
        }

        private void DoUnhide()
        {
            var process = (Process)processDataGrid.SelectedItem;

            if (process != null)
                driver.ThreadUnhideFromDebugger(process.Id);
        }
    }
}
