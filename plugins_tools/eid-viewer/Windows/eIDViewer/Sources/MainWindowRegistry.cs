using System;
using System.Windows.Threading;
using System.Runtime.InteropServices;
using System.Windows.Media.Imaging;
using Microsoft.Win32;
using System.Security;
using System.IO;
using System.Windows;
using System.Globalization;
using System.ComponentModel;

namespace eIDViewer
{

    public partial class MainWindow : Window
    {
        public void StoreLanguage(string language)
        {
            theBackendData.WriteRegistryStringValue("SOFTWARE\\BEID\\general", "language", language);
        }

        public void StoreStartupVersionCheck(int startupCheck)
        {
            theBackendData.WriteRegistryDwordValue("SOFTWARE\\BEID\\eidviewer", "startup_version_check", startupCheck);
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
            string readValue = theBackendData.ReadRegistryStringValue("SOFTWARE\\BEID\\general", "language", null);
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
