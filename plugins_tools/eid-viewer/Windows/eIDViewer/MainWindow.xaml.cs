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
using System.Globalization;

namespace eIDViewer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()    
        {
            InitializeComponent();


        }
        void ExitMenuItem_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void ChangeLocalization (string localization)
        {
            Thread.CurrentThread.CurrentCulture = new CultureInfo(localization);
            Thread.CurrentThread.CurrentUICulture = new CultureInfo(localization);
            eIDViewer.LocalizedStrings theLocalizedStrings = (LocalizedStrings)(App.Current.Resources["LocalizedStrings"]);
            theLocalizedStrings.resource1 = new eIDViewer.Resources.ApplicationStringResources();
        }

        private void Deutch_Click(object sender, RoutedEventArgs e)
        {
            ChangeLocalization("de-DE");
        }

        private void English_Click(object sender, RoutedEventArgs e)
        {
            ChangeLocalization("en-US");
        }

        private void French_Click(object sender, RoutedEventArgs e)
        {
            ChangeLocalization("fr-BE");
        }

        private void Nederlands_Click(object sender, RoutedEventArgs e)
        {
            ChangeLocalization("nl-BE");
        }
    }
}
