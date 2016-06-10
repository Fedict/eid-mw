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
using System.IO;
using Microsoft.Win32;
using System.Windows.Controls.Primitives;

namespace eIDViewer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        eIDViewer.BackendDataViewModel theBackendData = (BackendDataViewModel)(App.Current.Resources["eIDViewerBackendObj"]);

        public MainWindow()    
        {
            InitializeComponent();
            GetLanguage();
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
            
            theLocalizedStrings.NotifyPropertyChanged("identityTabResource");
            theLocalizedStrings.NotifyPropertyChanged("cardTabResource");
            theLocalizedStrings.NotifyPropertyChanged("certificateTabResource");
            theLocalizedStrings.NotifyPropertyChanged("applicationResource");
            theLocalizedStrings.NotifyPropertyChanged("menuItemResource");
          
            theBackendData.Refresh();
        }

        private void Deutch_Click(object sender, RoutedEventArgs e)
        {
            ChangeLocalization("de-DE");
            StoreLanguage("de");
            eIDViewer.NativeMethods.ChangeLanguage(eid_vwr_langs.EID_VWR_LANG_DE);
        }

        private void English_Click(object sender, RoutedEventArgs e)
        {
            ChangeLocalization("en-US");
            StoreLanguage("en");
            eIDViewer.NativeMethods.ChangeLanguage(eid_vwr_langs.EID_VWR_LANG_EN);
        }

        private void French_Click(object sender, RoutedEventArgs e)
        {
            ChangeLocalization("fr-BE");
            StoreLanguage("fr");
            eIDViewer.NativeMethods.ChangeLanguage(eid_vwr_langs.EID_VWR_LANG_FR);
        }

        private void Nederlands_Click(object sender, RoutedEventArgs e)
        {
            ChangeLocalization("nl-BE");
            StoreLanguage("nl");
            eIDViewer.NativeMethods.ChangeLanguage(eid_vwr_langs.EID_VWR_LANG_NL);
        }

        private void ClearLog_Click(object sender, RoutedEventArgs e)
        {
            eIDViewer.BackendDataViewModel theBackendData = (BackendDataViewModel)(App.Current.Resources["eIDViewerBackendObj"]);
            theBackendData.logText = "";
        }

        void LogLevel_OnSelectionChanged(object sender, RoutedEventArgs e)
        {
            ComboBox logCombo = sender as ComboBox;
            if(logCombo != null)
            {
                eIDViewer.BackendDataViewModel theBackendData = (BackendDataViewModel)(App.Current.Resources["eIDViewerBackendObj"]);

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
            eIDViewer.BackendDataViewModel theBackendData = (BackendDataViewModel)(App.Current.Resources["eIDViewerBackendObj"]);
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

        private void MenuItemOpen_Click(object sender, RoutedEventArgs e)
        {
            //Stream myStream = null;
            String filename = null;
            OpenFileDialog myOpenFileDialog = new OpenFileDialog();

            myOpenFileDialog.Filter = "eid files (*.eid)|*.eid|All files (*.*)|*.*";
            myOpenFileDialog.FilterIndex = 1;

            if (myOpenFileDialog.ShowDialog() == true)
            {
                try
                {
                    if ((filename = myOpenFileDialog.FileName) != null)
                    {
                        MessageBox.Show("File selected is " + filename);
                        eIDViewer.NativeMethods.OpenXML(filename);
                    }
                   /* if ((myStream = myOpenFileDialog.OpenFile()) != null)
                    {
                        using (myStream)
                        {
                            int length = (int)myStream.Length;
                            byte[] buffer = new byte[length];
                            int bytesRead = 0;
                            do
                            {
                                bytesRead = myStream.Read(buffer, 0, length - bytesRead);
                            } while (bytesRead > 0);

                        }
                    }*/
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error: Could not read file from disk. Error message: " + ex.Message);
                }
            }
        }
    private void MenuItemSaveAs_Click(object sender, RoutedEventArgs e)
    {
        //Stream myStream = null;
        String filename = null;
        SaveFileDialog mySaveFileDialog = new SaveFileDialog();

       mySaveFileDialog.Filter = "eid files (*.eid)|*.eid|All files (*.*)|*.*";
       mySaveFileDialog.FilterIndex = 1;

       if (mySaveFileDialog.ShowDialog() == true)
       {
           try
           {
               if ((filename = mySaveFileDialog.FileName) != null)
               {
                   MessageBox.Show("File selected is " + filename);
                   eIDViewer.NativeMethods.SaveXML(filename);
               }
           }
           catch (Exception ex)
               {
                   MessageBox.Show("Error: Could not read file from disk. Error message: " + ex.Message);
               }
       }
    }

        private void checkBox_Checked(object sender, RoutedEventArgs e)
        {

        }

        private void TextBlockCertificate_GotFocus(object sender, RoutedEventArgs e)
        {
            TextBlock certText = sender as TextBlock;

        }

        private void TestSiteMenu_Click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start("http://test.eid.belgium.be");
        }

        private void FAQMenuItem_Click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start("http://faq.eid.belgium.be/");
        }

        private void AboutMenuItem_Click(object sender, RoutedEventArgs e)
        {
            Popup myPopup = new Popup();
            myPopup.IsOpen = true;
            
            string message = @"eID Viewer 4.2.0
eID Middleware Project
Copyright(C) 2016 Fedict

By Frederik Vernelen and Wouter Verhelst
Based on the design of Frank Marien

This is free software; you can redistribute it and / or modify it
under the terms of the GNU Lesser General Public License version 
3.0 as published by the Free Software Foundation.

This software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU 
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public 
License along with this software; if not, see 
http://www.gnu.org/licenses

Official releases and support are available on http://eid.belgium.be
Source code and other files are available on https://github.com/Fedict/eid-viewer";
            string caption = "About";
            MessageBoxResult result = new MessageBoxResult();
            MessageBox.Show(message, caption, MessageBoxButton.OK, MessageBoxImage.Information, result);
        }
    }

}
