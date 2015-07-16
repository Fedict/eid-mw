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
using System.Threading;

namespace eIDViewer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()    
        {
            m_eIDViewerBackend = new eIDViewerBackend();
            m_eIDViewerBackend.Init();

            Thread backendThread = new Thread(m_eIDViewerBackend.backendMainloop);
            backendThread.Start();
            Console.WriteLine("backendThread started");

            InitializeComponent();
            
        }
        void ExitMenuItem_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private eIDViewerBackend m_eIDViewerBackend;
    }
}
