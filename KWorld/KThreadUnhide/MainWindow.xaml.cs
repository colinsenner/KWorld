using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Diagnostics;

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
            processListView.ItemsSource = null;
            processListView.Items.Clear();

            processListView.ItemsSource = Process.GetProcesses();
        }

        private void refreshButton_Click(object sender, RoutedEventArgs e)
        {
            PopulateProcesses();
        }

        private void unhideButton_Click(object sender, RoutedEventArgs e)
        {

        }

        private void ListView_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            UpdateColumnsWidth(sender as ListView);
        }

        private void ListView_Loaded(object sender, RoutedEventArgs e)
        {
            UpdateColumnsWidth(sender as ListView);
        }

        private void UpdateColumnsWidth(ListView listView)
        {
            int autoFillColumnIndex = (listView.View as GridView).Columns.Count - 1;
            if (listView.ActualWidth == double.NaN)
                listView.Measure(new Size(double.PositiveInfinity, double.PositiveInfinity));
            double remainingSpace = listView.ActualWidth - 28;
            for (int i = 0; i < (listView.View as GridView).Columns.Count; i++)
                if (i != autoFillColumnIndex)
                    remainingSpace -= (listView.View as GridView).Columns[i].ActualWidth;
            (listView.View as GridView).Columns[autoFillColumnIndex].Width = remainingSpace >= 0 ? remainingSpace : 0;
        }
    }
}
