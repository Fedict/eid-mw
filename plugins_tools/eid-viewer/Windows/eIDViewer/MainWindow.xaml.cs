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
using System.Xml;
using System.Net;

namespace eIDViewer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        eIDViewer.BackendDataViewModel theBackendData = (BackendDataViewModel)(App.Current.Resources["eIDViewerBackendObj"]);
        private MenuItem LastCardReaderMenuItem = null;
        public static RoutedCommand SelectGermanCommand = new RoutedCommand();
        public static RoutedCommand SelectEnglishCommand = new RoutedCommand();
        public static RoutedCommand SelectFrenchCommand = new RoutedCommand();
        public static RoutedCommand SelectDutchCommand = new RoutedCommand();
        public static RoutedCommand SelectOpenCommand = new RoutedCommand();
        public static RoutedCommand SelectSaveCommand = new RoutedCommand();
        public static RoutedCommand SelectCloseCommand = new RoutedCommand();
        public static RoutedCommand SelectPrintCommand = new RoutedCommand();
        public static RoutedCommand SelectQuitCommand = new RoutedCommand();

        public MainWindow()
        {
            InitializeComponent();
            GetLanguage();

            this.Loaded += new RoutedEventHandler(MainWindow_Loaded);
            this.Loaded += new RoutedEventHandler(Version_Check);
        }

        void Version_Check(object sender, RoutedEventArgs e)
        {
            try
            {
                Popup myPopup = new Popup();
                myPopup.IsOpen = true;

                string url = "";
                string releaseNotes = "";
                int regVal = 0;
                theBackendData.startupVersionCheck = false;

                regVal = theBackendData.ReadRegistryDwordValue("SOFTWARE\\BEID\\eidviewer", "startup_version_check", 2);
                if ((regVal == 2) || (regVal == -1))
                {
                    //2 is the default value we send (reg key name value not found), -1 means the reg key does not exist, so try to create the key name with value 1
                    theBackendData.WriteRegistryDwordValue("SOFTWARE\\BEID\\eidviewer", "startup_version_check", 1);
                    if (theBackendData.ReadRegistryDwordValue("SOFTWARE\\BEID\\eidviewer", "startup_version_check", 2) != 1)
                    {
                        //if we cannot write/read the register, we won't be able to disable the version check, so skip it
                        return;
                    }
                }
                else if (regVal <= 0)
                {
                    //either it is not needed (regVal == 0) or we failed reading registry (regVal < 0), so skip the version check
                    return;
                }

                //Do the version check, as the "SOFTWARE\BEID\eidviewer\startup_version_check" registry value (DWORD) is set
                theBackendData.startupVersionCheck = true;
                theBackendData.WriteLog("starting the online version check..\n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);

                string language = CultureInfo.CurrentUICulture.TwoLetterISOLanguageName.ToString();

                if (!Version.getUpdateUrl(out bool updateNeeded, language, ref url, ref releaseNotes))
                {
                    //no updated version found, report this in the log
                    theBackendData.WriteLog("failed to check for online update\n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);
                    return;
                }

                if (updateNeeded)
                {
                    if (url == "")
                    {
                        theBackendData.WriteLog("A newer version of the eID Viewer has been found, but not the url to download it\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                        theBackendData.WriteLog("Newer version should be available at https://eid.belgium.be\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                        return;
                    }
                    //for safety, we do not accept all urls
                    else if (url.StartsWith("https://eid.belgium.be"))
                    {
                        //xml Version check
                        string aboutMessage = "\n" + eIDViewer.Resources.ApplicationStringResources.newVersionDownload;
                        string caption = eIDViewer.Resources.ApplicationStringResources.newVersionDownloadTitle;

                        MessageBoxResult result = MessageBox.Show(aboutMessage, caption, MessageBoxButton.OKCancel, MessageBoxImage.Information, MessageBoxResult.Cancel);
                        if (result == MessageBoxResult.OK)
                        {
                            try
                            {
                                System.Diagnostics.Process.Start(url);
                            }
                            catch (Exception ex)
                            {
                                theBackendData.WriteLog("Error: Could not start a browser to visit" + url + ex.Message, eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                            }
                        }
                    }
                }
                else
                {
                    theBackendData.WriteLog("No viewer update is needed\n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error occured during eID Viewer version check" + ex.Message);
            }
        }

        void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            try
            {
                if (Application.Current.Properties["eidFileToOpen"] != null)
                {
                    string eIDFile = Application.Current.Properties["eidFileToOpen"].ToString();
                    if (eIDFile != null)
                    {
                        //close previous file
                        eIDViewer.NativeMethods.CloseXML();
                        //MessageBox.Show("File selected is " + filename);
                        eIDViewer.NativeMethods.OpenXML(eIDFile);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: Could not read file that started the eIDViewer. Error message: " + ex.Message);
            }
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
            theBackendData.logText = "";
        }

        void LogLevel_OnSelectionChanged(object sender, RoutedEventArgs e)
        {
            ComboBox logCombo = sender as ComboBox;
            if (logCombo != null)
            {
                theBackendData.WriteLog("log_level combo selected\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);

                switch (logCombo.SelectedIndex)
                {
                    //only save changes if they are not set yet (this event also gets fired during initialization)
                    case 0:
                        if (theBackendData.log_level != eid_vwr_loglevel.EID_VWR_LOG_ERROR)
                        {
                            theBackendData.log_level = eid_vwr_loglevel.EID_VWR_LOG_ERROR;
                            theBackendData.StoreViewerLogLevel("Error");
                            theBackendData.WriteLog("switched log_level to Error\n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);
                        }
                        break;
                    case 1:
                        if (theBackendData.log_level != eid_vwr_loglevel.EID_VWR_LOG_COARSE)
                        {
                            theBackendData.log_level = eid_vwr_loglevel.EID_VWR_LOG_COARSE;
                            theBackendData.StoreViewerLogLevel("Warning");
                            theBackendData.WriteLog("switched log_level to Warning\n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);
                        }
                        break;
                    case 2:
                        if (theBackendData.log_level != eid_vwr_loglevel.EID_VWR_LOG_NORMAL)
                        {
                            theBackendData.log_level = eid_vwr_loglevel.EID_VWR_LOG_NORMAL;
                            theBackendData.StoreViewerLogLevel("Info");
                            theBackendData.WriteLog("switched log_level to Info\n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);
                        }
                        break;
                    case 3:
                        if (theBackendData.log_level != eid_vwr_loglevel.EID_VWR_LOG_DETAIL)
                        {
                            theBackendData.log_level = eid_vwr_loglevel.EID_VWR_LOG_DETAIL;
                            theBackendData.StoreViewerLogLevel("Debug");
                            theBackendData.WriteLog("switched log_level to Debug\n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);
                        }
                        break;
                    default:
                        {
                            theBackendData.WriteLog("invalid index of log_level combo selected\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                            break;
                        }
                }
            }
        }

        private void CopyLogToClipboard_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                System.Windows.Clipboard.SetText(theBackendData.logText);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: Copy log to clipboard failed. Error message: " + ex.Message);
            }
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

            try
            {
                OpenFileDialog myOpenFileDialog = new OpenFileDialog();

                myOpenFileDialog.Filter = "eid files (*.eid)|*.eid|All files (*.*)|*.*";
                myOpenFileDialog.FilterIndex = 1;

                if (myOpenFileDialog.ShowDialog() == true)
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
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: Could not read file from disk. Error message: " + ex.Message);
            }
        }
        private void MenuItemSaveAs_Click(object sender, RoutedEventArgs e)
        {
            //We allow choosing the file to save to already before the card has been fully read
            //(for time saving purposes)
            String filename = null;

            try
            {
                SaveFileDialog mySaveFileDialog = new SaveFileDialog();

                mySaveFileDialog.Filter = "eid files (*.eid)|*.eid|All files (*.*)|*.*";
                mySaveFileDialog.FilterIndex = 1;

                if (mySaveFileDialog.ShowDialog() == true)
                {

                    if ((filename = mySaveFileDialog.FileName) != null)
                    {
                        if (theBackendData.eid_data_ready == true)
                        {
                            eIDViewer.NativeMethods.SaveXML(filename);
                        }
                        else
                        {
                            MessageBox.Show("Error: Data not ready yet, file not saved ");
                        }
                    }

                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: Could not save file to disk. Error message: " + ex.Message);
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
            try
            {
                System.Diagnostics.Process.Start("https://eid.belgium.be");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: Could not start a browser to visit https://eid.belgium.be" + ex.Message);
            }
        }

        private void FAQMenuItem_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                System.Diagnostics.Process.Start("https://eid.belgium.be");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: Could not start a browser to visit https://eid.belgium.be" + ex.Message);
            }
        }

        private void AboutMenuItem_Click(object sender, RoutedEventArgs e)
        {
            Popup myPopup = new Popup();
            myPopup.IsOpen = true;

            string message = About.AboutMessage();
            string caption = "About";
            MessageBoxResult result = new MessageBoxResult();
            MessageBox.Show(message, caption, MessageBoxButton.OK, MessageBoxImage.Information, result);
        }

        private void MenuItemPrint_Click(object sender, RoutedEventArgs e)
        {

            theBackendData.date = DateTime.Now.ToString("D",Thread.CurrentThread.CurrentUICulture);

            PrintWindow thePrintWindow = new PrintWindow();
            PrintDialog dialog = new PrintDialog();

            try
            {
                if (dialog.ShowDialog() != true)
                    return;

                System.Printing.PrintCapabilities capabilities = dialog.PrintQueue.GetPrintCapabilities(dialog.PrintTicket);

                Point printMargin = new Point(capabilities.PageImageableArea.OriginWidth, capabilities.PageImageableArea.OriginHeight);
                Size printSize = new Size(capabilities.PageImageableArea.ExtentWidth, capabilities.PageImageableArea.ExtentHeight);

                theBackendData.WriteLog("capabilities.PageImageableArea.OriginWidth = " + capabilities.PageImageableArea.OriginWidth + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
                theBackendData.WriteLog("capabilities.PageImageableArea.OriginHeight = " + capabilities.PageImageableArea.OriginHeight + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
                theBackendData.WriteLog("capabilities.PageImageableArea.ExtentWidth = " + capabilities.PageImageableArea.ExtentWidth + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
                theBackendData.WriteLog("capabilities.PageImageableArea.ExtentHeight = " + capabilities.PageImageableArea.ExtentHeight + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);

                //Size printSize = new Size(dialog.PrintableAreaWidth, dialog.PrintableAreaHeight);
                //thePrintWindow.printWindowGrid.Measure(printSize);
                thePrintWindow.printWindowGrid.Arrange(new Rect(printMargin, printSize));

                dialog.PrintVisual(thePrintWindow.printWindowGrid, "Printing");
                thePrintWindow.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: " + ex.Message);
                thePrintWindow.Close();
            }
        }

        private void ValidateNowButton_Click(object sender, RoutedEventArgs e)
        {
            theBackendData.VerifyAllCertificates();
        }

        public static string Utf8ToString(IntPtr nativeUtf8)
        {
            int len = 0;
            try
            {
                while (Marshal.ReadByte(nativeUtf8, len) != 0)
                {
                    len++;
                }
                byte[] buffer = new byte[len];
                Marshal.Copy(nativeUtf8, buffer, 0, buffer.Length);
                return Encoding.UTF8.GetString(buffer);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: converting utf8 to string failed" + ex.Message);
                return "";
            }
        }

        private void eIDPictureDnD(object sender, MouseEventArgs e)
        {
            try
            {
                if (e.Source.GetType().Name.Equals("Image"))
                {
                    if ((theBackendData.eid_backend_state == eid_vwr_states.STATE_FILE_WAIT) || (theBackendData.eid_backend_state == eid_vwr_states.STATE_TOKEN_IDLE))
                    {
                        Image item = (Image)e.Source;

                        //STATE_FILE or STATE_TOKEN_WAIT
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
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: Drag and Drop failed" + ex.Message);
            }
        }

        private void CardReaderMenuItem_Click(object sender, RoutedEventArgs e)
        {
            if (e.Source.GetType().Name.Equals("MenuItem"))
            {
                MenuItem menu = e.Source as MenuItem;
                if ((LastCardReaderMenuItem != null) && (LastCardReaderMenuItem != menu))
                {
                    LastCardReaderMenuItem.IsChecked = false;
                }
                else if (menu.IsChecked == false)
                {
                    //if the previous selected card reader is deselected, go back to auto mode
                    eIDViewer.NativeMethods.SelectCardReader(1, 0);
                    return;
                }

                LastCardReaderMenuItem = menu;

                if (menu.DataContext.GetType().Name.Equals("ReadersMenuViewModel"))
                {
                    ReadersMenuViewModel reader = menu.DataContext as ReadersMenuViewModel;
                    eIDViewer.NativeMethods.SelectCardReader(0, reader.slotNumber);
                }
            }
        }

        private void VersionMenuItem_Click(object sender, RoutedEventArgs e)
        {
            //switch the startupVersionCheck setting and store it in the registry
            if (theBackendData.startupVersionCheck == true)
            {
                if (StoreStartupVersionCheck(0))
                {
                    theBackendData.startupVersionCheck = false; 
                }                
                else
                {
                    //storing the registry value failed, make sure the UI reflects this
                    theBackendData.startupVersionCheck = true;
                    theBackendData.WriteLog("Failed storing the startupversioncheck value in the registry, so not changing it\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                }
            }
            else
            {
                if(StoreStartupVersionCheck(1))
                {
                    theBackendData.startupVersionCheck = true;
                }
                else
                {
                    //storing the registry value failed, make sure the UI reflects this
                    theBackendData.startupVersionCheck = false;
                    theBackendData.WriteLog("Failed storing the startupversioncheck value in the registry, so not changing it\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                }                
            }
        }
    }
}

