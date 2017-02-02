using System;
using System.Windows.Threading;
using System.Runtime.InteropServices;
using System.Windows.Media.Imaging;
using Microsoft.Win32;
using System.Security;
using System.IO;
using System.Windows;
using System.Globalization;

namespace eIDViewer
{

    public partial class MainWindow : Window
    {  
        string ReadRegistryStringValue(string subkey, string valueName, object defaultValue)
        {
            const string userRoot = "HKEY_CURRENT_USER";
            const string localMachineRoot = "HKEY_LOCAL_MACHINE";
            string keyName = userRoot + "\\" + subkey;
            try
            {

                object readValue = Registry.GetValue(keyName, valueName, null);

                if (readValue == null)
                {
                    keyName = localMachineRoot + "\\" + subkey;
                    readValue = Registry.GetValue(keyName, valueName, null);
                }
                if (readValue == null)
                {
                    return "";
                }
                else
                {
                    return readValue as string;
                }
            }
            catch (SecurityException e)
            {
                theBackendData.logText += "ReadRegistryStringValue failed, no permission to read key " + keyName;
                theBackendData.logText += "Exception message: " + e.Message + "\n";
                return "";
            }
            catch (IOException e)
            {
                theBackendData.logText += "ReadRegistryStringValue failed, the key " + keyName + " was marked for deletion";
                theBackendData.logText += "Exception message: " + e.Message + "\n";
                return "";
            }
            catch (Exception e)
            {
                theBackendData.logText += "Exception message: " + e.Message + "\n";
                return "";
            }
        }

        void WriteRegistryStringValue(string subkey, string valueName, object valueValue)
        {
            const string userRoot = "HKEY_CURRENT_USER";
            string keyName = userRoot + "\\" + subkey;
            try
            {
                Registry.SetValue(keyName, valueName, valueValue as string, RegistryValueKind.String);
            }

            catch (Exception e)
            {
               theBackendData.logText += "Exception caught when trying to write to registry key " + keyName;
               Console.WriteLine("{0} Exception caught.", e);
            }
        }

        public void StoreLanguage (string language)
        {
            WriteRegistryStringValue("SOFTWARE\\BEID\\general", "language", language);
        }

        public void SetLanguageNL()
        {
            theBackendData.SetLanguage(eid_vwr_langs.EID_VWR_LANG_NL);
            ChangeLocalization("nl-BE");
            eIDViewer.NativeMethods.ChangeLanguage(eid_vwr_langs.EID_VWR_LANG_NL);
        }

        public void SetLanguageFR()
        {
            theBackendData.SetLanguage(eid_vwr_langs.EID_VWR_LANG_FR);
            ChangeLocalization("fr-BE");
            eIDViewer.NativeMethods.ChangeLanguage(eid_vwr_langs.EID_VWR_LANG_FR);
        }

        public void SetLanguageDE()
        {
            theBackendData.SetLanguage(eid_vwr_langs.EID_VWR_LANG_DE);
            ChangeLocalization("de-DE");
            eIDViewer.NativeMethods.ChangeLanguage(eid_vwr_langs.EID_VWR_LANG_DE);
        }

        public void SetLanguageEN()
        {
            theBackendData.SetLanguage(eid_vwr_langs.EID_VWR_LANG_EN);
            ChangeLocalization("en-US");
            eIDViewer.NativeMethods.ChangeLanguage(eid_vwr_langs.EID_VWR_LANG_EN);
        }

        public void SetSystemLanguage()
        {
            if (CultureInfo.InstalledUICulture.TwoLetterISOLanguageName.Equals("nl"))
            {
                SetLanguageNL();
            }
            else if (CultureInfo.InstalledUICulture.TwoLetterISOLanguageName.Equals("fr"))
            {
                SetLanguageFR();
            }
            else if (CultureInfo.InstalledUICulture.TwoLetterISOLanguageName.Equals("de"))
            {
                SetLanguageDE();
            }
            else if (CultureInfo.InstalledUICulture.TwoLetterISOLanguageName.Equals("en"))
            {
                SetLanguageEN();
            }
            else
            {
                theBackendData.logText += "unsupported system language: " + CultureInfo.InstalledUICulture.TwoLetterISOLanguageName;
                theBackendData.logText += "switching language to english ";
                SetLanguageEN();
            }
        }

        public void GetLanguage()
        {
            string readValue = ReadRegistryStringValue("SOFTWARE\\BEID\\general", "language", null);
            if(readValue != null)
            {
                if( readValue.Equals("nl"))
                {
                    SetLanguageNL();
                }
                else if (readValue.Equals("fr"))
                {
                    SetLanguageFR();
                }
                else if (readValue.Equals("de"))
                {
                    SetLanguageDE();
                }
                else if (readValue.Equals("en"))
                {
                    SetLanguageEN();
                }
                else
                {
                    theBackendData.logText += "unknown language identifier found in registry: " + readValue;
                    theBackendData.logText += "switching language to the system setting ";
                    SetSystemLanguage();
                }
            }
            else
            {
                theBackendData.logText += "no language identifier found in registry, using the system localization ";
                SetSystemLanguage();
            }
        }

    }

}
