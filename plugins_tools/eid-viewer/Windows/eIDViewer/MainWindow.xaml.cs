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
using System.Runtime.InteropServices;

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

        private void ChangeLocalization(string localization)
        {
            Thread.CurrentThread.CurrentCulture = new CultureInfo(localization);
            Thread.CurrentThread.CurrentUICulture = new CultureInfo(localization);
            eIDViewer.LocalizedStrings theLocalizedStrings = (LocalizedStrings)(App.Current.Resources["LocalizedStrings"]);

            theLocalizedStrings.NotifyPropertyChanged("identityTabResource");
            theLocalizedStrings.NotifyPropertyChanged("cardTabResource");
            theLocalizedStrings.NotifyPropertyChanged("certificateTabResource");
            theLocalizedStrings.NotifyPropertyChanged("applicationResource");
            theLocalizedStrings.NotifyPropertyChanged("menuItemResource");

        }

        private void Deutch_Click(object sender, RoutedEventArgs e)
        {
            //Change the Localization of the language in the UI
            ChangeLocalization("de-DE");
            //Store the chosen language in the registry
            StoreLanguage("de");
            //show the language marker in the menu
            theBackendData.SetLanguage(eid_vwr_langs.EID_VWR_LANG_DE);
            //tell the backend to switch the language
            eIDViewer.NativeMethods.ChangeLanguage(eid_vwr_langs.EID_VWR_LANG_DE);
        }

        private void English_Click(object sender, RoutedEventArgs e)
        {
            //Change the Localization of the language in the UI
            ChangeLocalization("en-US");
            //Store the chosen language in the registry
            StoreLanguage("en");
            //show the language marker in the menu
            theBackendData.SetLanguage(eid_vwr_langs.EID_VWR_LANG_EN);
            //tell the backend to switch the language
            eIDViewer.NativeMethods.ChangeLanguage(eid_vwr_langs.EID_VWR_LANG_EN);
        }

        private void French_Click(object sender, RoutedEventArgs e)
        {
            //Change the Localization of the language in the UI
            ChangeLocalization("fr-BE");
            //Store the chosen language in the registry
            StoreLanguage("fr");
            //show the language marker in the menu
            theBackendData.SetLanguage(eid_vwr_langs.EID_VWR_LANG_FR);
            //tell the backend to switch the language
            eIDViewer.NativeMethods.ChangeLanguage(eid_vwr_langs.EID_VWR_LANG_FR);
        }

        private void Nederlands_Click(object sender, RoutedEventArgs e)
        {
            //Change the Localization of the language in the UI
            ChangeLocalization("nl-BE");
            //Store the chosen language in the registry
            StoreLanguage("nl");
            //show the language marker in the menu
            theBackendData.SetLanguage(eid_vwr_langs.EID_VWR_LANG_NL);
            //tell the backend to switch the language
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
            if (logCombo != null)
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
                        //close previous file
                        eIDViewer.NativeMethods.CloseXML();
                        //MessageBox.Show("File selected is " + filename);
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
                        eIDViewer.NativeMethods.SaveXML(filename);
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error: Could not read file from disk. Error message: " + ex.Message);
                }
            }
        }


        private void MenuItemClose_Click(object sender, RoutedEventArgs e)
        {
            eIDViewer.NativeMethods.CloseXML();
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

        private void MenuItemPrint_Click(object sender, RoutedEventArgs e)
        {
            eIDViewer.BackendDataViewModel theBackendData = (BackendDataViewModel)(App.Current.Resources["eIDViewerBackendObj"]);

            theBackendData.date = DateTime.Now.ToString("D",Thread.CurrentThread.CurrentUICulture);

            PrintWindow thePrintWindow = new PrintWindow();
            PrintDialog dialog = new PrintDialog();

            if (dialog.ShowDialog() != true)
                return;

            //re-arrange the printwindow grid to fill the entire page
            thePrintWindow.printWindowGrid.Measure(new Size(dialog.PrintableAreaWidth, dialog.PrintableAreaHeight));
            thePrintWindow.printWindowGrid.Arrange(new Rect(new Point(0, 0), thePrintWindow.printWindowGrid.DesiredSize));

            dialog.PrintVisual(thePrintWindow.printWindowGrid, "Printing");
            thePrintWindow.Close();
        }

        private void ValidateNowButton_Click(object sender, RoutedEventArgs e)
        {
            eIDViewer.BackendDataViewModel theBackendData = (BackendDataViewModel)(App.Current.Resources["eIDViewerBackendObj"]);
            theBackendData.VerifyAllCertificates();
        }

        public static string Utf8ToString(IntPtr nativeUtf8)
        {
            int len = 0;
            while (Marshal.ReadByte(nativeUtf8, len) != 0)
            {
                len++;
            }
            byte[] buffer = new byte[len];
            Marshal.Copy(nativeUtf8, buffer, 0, buffer.Length);
            return Encoding.UTF8.GetString(buffer);
        }

        private void eIDPictureDnD(object sender, MouseEventArgs e)
        {
            if (e.Source.GetType().Name.Equals("Image"))
            {
                Image item = (Image)e.Source;

                if (item != null)
                {
                    IntPtr intptrXML = eIDViewer.NativeMethods.GetXMLForm();
                    string XMLForm = Utf8ToString(intptrXML);

                    DataObject dataObject = new DataObject();
                    dataObject.SetData(DataFormats.StringFormat, XMLForm.ToString());
                    DragDrop.DoDragDrop(item, dataObject, DragDropEffects.Copy);
                }
            }
        }

        private void CardReaderMenuItem_Click(object sender, RoutedEventArgs e)
        {
            if (e.Source.GetType().Name.Equals("MenuItem"))
            {
                MenuItem menu = e.Source as MenuItem;
                if (menu.DataContext.GetType().Name.Equals("ReadersMenuViewModel"))
                {
                    ReadersMenuViewModel reader = menu.DataContext as ReadersMenuViewModel;
                    eIDViewer.NativeMethods.SelectCardReader(0, reader.slotNumber);
                }
            }
        }

    }
}

