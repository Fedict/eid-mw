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
            theLocalizedStrings.identityTabResource = new eIDViewer.Resources.IdentityTabStringResources();
            theLocalizedStrings.applicationResource = new eIDViewer.Resources.ApplicationStringResources();

            eIDViewer.eIDViewerBackendData theBackendData = (eIDViewerBackendData)(App.Current.Resources["eIDViewerBackendObj"]);
            theBackendData.Refresh();
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

        private void ClearLog_Click(object sender, RoutedEventArgs e)
        {
            eIDViewer.eIDViewerBackendData theBackendData = (eIDViewerBackendData)(App.Current.Resources["eIDViewerBackendObj"]);
            theBackendData.logText = "";
        }

        void LogLevel_OnSelectionChanged(object sender, RoutedEventArgs e)
        {
            ComboBox logCombo = sender as ComboBox;
            if(logCombo != null)
            {
                eIDViewer.eIDViewerBackendData theBackendData = (eIDViewerBackendData)(App.Current.Resources["eIDViewerBackendObj"]);

                switch (logCombo.SelectedIndex)
                {
                    case 0:
                        theBackendData.log_level = eid_vwr_loglevel.EID_VWR_LOG_ERROR;
                        break;
                    case 1:
                        theBackendData.log_level = eid_vwr_loglevel.EID_VWR_LOG_COARSE;
                        break;
                    case 2:
                        theBackendData.log_level = eid_vwr_loglevel.EID_VWR_LOG_NORMAL;
                        break;
                    case 3:
                        theBackendData.log_level = eid_vwr_loglevel.EID_VWR_LOG_DETAIL;
                        break;
                    default:
                        break;
                }
            }
        }

        private void CopyLogToClipboard_Click(object sender, RoutedEventArgs e)
        {
            eIDViewer.eIDViewerBackendData theBackendData = (eIDViewerBackendData)(App.Current.Resources["eIDViewerBackendObj"]);
            System.Windows.Clipboard.SetText(theBackendData.logText);
        }

        private void PincodeTest_Click(object sender, RoutedEventArgs e)
        {
            eIDViewer.NativeMethods.DoPinop(eid_vwr_pinops.EID_VWR_PINOP_TEST);
        }

        private void PincodeTest_Change(object sender, RoutedEventArgs e)
        {
            eIDViewer.NativeMethods.DoPinop(eid_vwr_pinops.EID_VWR_PINOP_CHG);
        }
    }
}
