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

    public partial class BackendDataViewModel : INotifyPropertyChanged
    {
        public string ReadRegistryStringValue(string subkey, string valueName, object defaultValue)
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
                this.WriteLog("ReadRegistryStringValue failed, no permission to read key " + keyName, eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                this.WriteLog("Exception message: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                return "";
            }
            catch (IOException e)
            {
                this.WriteLog("ReadRegistryStringValue failed, the key " + keyName + " was marked for deletion", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                this.WriteLog("Exception message: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                return "";
            }
            catch (Exception e)
            {
                this.WriteLog("Exception message: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                return "";
            }
        }

        public void WriteRegistryStringValue(string subkey, string valueName, object valueValue)
        {
            const string userRoot = "HKEY_CURRENT_USER";
            string keyName = userRoot + "\\" + subkey;
            try
            {
                Registry.SetValue(keyName, valueName, valueValue as string, RegistryValueKind.String);
            }

            catch (Exception e)
            {
                this.WriteLog("Exception" + e.Message + " caught when trying to write to registry key " + keyName, eid_vwr_loglevel.EID_VWR_LOG_COARSE);
            }
        }

        public void StoreViewerLogLevel(string viewerloglevel)
        {
            WriteRegistryStringValue("SOFTWARE\\BEID\\logging", "eidviewer_log_level", viewerloglevel);
        }

        public void GetViewerLogLevel()
        {
            try
            {
                string readValue = ReadRegistryStringValue("SOFTWARE\\BEID\\logging", "eidviewer_log_level", null);
                if (readValue != null)
                {
                    if (readValue.Equals("Error", StringComparison.OrdinalIgnoreCase))
                    {
                        this.log_level = eid_vwr_loglevel.EID_VWR_LOG_ERROR;
                        this.log_level_index = 0;
                    }
                    else if (readValue.Equals("Warning", StringComparison.OrdinalIgnoreCase))
                    {
                        this.log_level = eid_vwr_loglevel.EID_VWR_LOG_COARSE;
                        this.log_level_index = 1;
                    }
                    else if (readValue.Equals("Info", StringComparison.OrdinalIgnoreCase))
                    {
                        this.log_level = eid_vwr_loglevel.EID_VWR_LOG_NORMAL;
                        this.log_level_index = 2;
                    }
                    else if (readValue.Equals("Debug", StringComparison.OrdinalIgnoreCase))
                    {
                        this.log_level = eid_vwr_loglevel.EID_VWR_LOG_DETAIL;
                        this.log_level_index = 3;
                    }
                    else
                    {
                        this.WriteLog("unknown eidviewer_log_level setting identifier found in registry: " + readValue + "\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                        this.WriteLog("Known values are: Error, Warning, Info, Debug\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                        this.WriteLog("switching viewer log level to the default setting (Error)\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                        this.log_level = eid_vwr_loglevel.EID_VWR_LOG_ERROR;
                    }
                }
                else
                {
                    this.WriteLog("no viewer log level setting found in registry, using the default (Error) \n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                    this.log_level = eid_vwr_loglevel.EID_VWR_LOG_ERROR;
                }
            }
            catch (Exception e)
            {
                this.WriteLog("Exception in function GetViewerLogLevel: " + e.Message, eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
        }

    }

}
