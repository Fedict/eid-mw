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

    public partial class BackendDataViewModel : INotifyPropertyChanged, IDisposable
    {
        //to check if Dispose has been called before
        private bool disposed = false;

        protected virtual void Dispose(bool disposing)
        {
            if (this.disposed == false)
            {
                //store application settings
                if (Properties.Settings.Default.AlwaysValidate != validateAlways)
                {
                    Properties.Settings.Default.AlwaysValidate = validateAlways;
                    Properties.Settings.Default.Save();
                }

                if (disposing)
                {
                    // dispose managed resources
                    if (authentication_cert != null)
                    {
                        authentication_cert.Dispose();
                    }
                    if (signature_cert != null)
                    {
                        signature_cert.Dispose();
                    }
                    if (rootCA_cert != null)
                    {
                        rootCA_cert.Dispose();
                    }
                    if (intermediateCA_cert != null)
                    {
                        intermediateCA_cert.Dispose();
                    }
                    if (RN_cert != null)
                    {
                        RN_cert.Dispose();
                    }
                }
                //Dispose only once
                disposed = true;
            }
            // free native resources
        }

        public void Dispose()
        {
            Dispose(true);
            // call GC.SupressFinalize to take this object off the finalization queue
            // and prevent finalization code for this object from executing a second time.
            GC.SuppressFinalize(this);
        }

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

        public int ReadRegistryDwordValue(string subkey, string valueName, int defaultValue)
        {
            const string userRoot = "HKEY_CURRENT_USER";
            const string localMachineRoot = "HKEY_LOCAL_MACHINE";
            string keyName = userRoot + "\\" + subkey;
            try
            {
                int inter = 0;
                object readValue = Registry.GetValue(keyName, valueName, null);

                if (readValue == null)
                {
                    keyName = localMachineRoot + "\\" + subkey;
                    readValue = Registry.GetValue(keyName, valueName, defaultValue);
                }
                if (readValue == null)
                {
                    return -1;
                }
                else
                {
                    if (Type.Equals(readValue.GetType(), inter.GetType()))
                    {
                        inter = (int)readValue;
                    }
                   return inter;
                }
            }
            catch (SecurityException e)
            {
                this.WriteLog("ReadRegistryStringValue failed, no permission to read key " + keyName, eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                this.WriteLog("Exception message: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                return -2;
            }
            catch (IOException e)
            {
                this.WriteLog("ReadRegistryStringValue failed, the key " + keyName + " was marked for deletion", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                this.WriteLog("Exception message: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                return -3;
            }
            catch (Exception e)
            {
                this.WriteLog("Exception message: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                return -4;
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

        public bool WriteRegistryDwordValue(string subkey, string valueName, int dwordValue)
        {
            string keyName = "HKEY_CURRENT_USER\\" + subkey;
            try
            {
                Registry.SetValue(keyName, valueName, dwordValue, RegistryValueKind.DWord);
                return true;
            }

            catch (Exception e)
            {
                this.WriteLog("Exception" + e.Message + " caught when trying to write to registry key " + keyName, eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                return false;
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
                    //set log_level to something <= warning so we are sure the next messages get logged.
                    this.log_level = eid_vwr_loglevel.EID_VWR_LOG_NORMAL;
                        this.WriteLog("unknown eidviewer_log_level setting identifier found in registry: " + readValue + "\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                        this.WriteLog("Known values are: Error, Warning, Info, Debug\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                    this.WriteLog("switching viewer log level to the default setting \"Error\"\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                        this.log_level = eid_vwr_loglevel.EID_VWR_LOG_ERROR;
                    this.log_level_index = 0;
                    //also try to write it into the registry
                    StoreViewerLogLevel("Error");
                    }
                }
                else
                {
                //set log_level to something <= warning so we are sure the next messages get logged.
                this.log_level = eid_vwr_loglevel.EID_VWR_LOG_NORMAL;
                this.WriteLog("no viewer log level setting found in registry, using the default \"Error\" \n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                    this.log_level = eid_vwr_loglevel.EID_VWR_LOG_ERROR;
                this.log_level_index = 0;
                //also try to write it into the registry
                StoreViewerLogLevel("Error");
                }
            }
            catch (Exception e)
            {
                this.WriteLog("Exception in function GetViewerLogLevel: " + e.Message, eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
        }

    }

}
