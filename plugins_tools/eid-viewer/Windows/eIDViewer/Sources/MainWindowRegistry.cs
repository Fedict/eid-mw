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
                theBackendData.WriteLog("ReadRegistryStringValue failed, no permission to read key " + keyName, eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                theBackendData.WriteLog("Exception message: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                return "";
            }
            catch (IOException e)
            {
                theBackendData.WriteLog("ReadRegistryStringValue failed, the key " + keyName + " was marked for deletion", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                theBackendData.WriteLog("Exception message: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                return "";
            }
            catch (Exception e)
            {
                theBackendData.WriteLog("Exception message: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
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
                theBackendData.WriteLog("Exception caught when trying to write to registry key " + keyName, eid_vwr_loglevel.EID_VWR_LOG_COARSE);
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
                theBackendData.WriteLog("unsupported system language: " + CultureInfo.InstalledUICulture.TwoLetterISOLanguageName + "\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                theBackendData.WriteLog("switching language to english \n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
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
                    theBackendData.WriteLog("unknown language identifier found in registry: " + readValue + "\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                    theBackendData.WriteLog("switching language to the system setting \n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                    SetSystemLanguage();
                }
            }
            else
            {
                theBackendData.WriteLog("no language identifier found in registry, using the system localization \n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                SetSystemLanguage();
            }
        }

    }

}
