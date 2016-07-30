using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Threading;

namespace eIDViewer
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private Thread backendThread;

        protected override void OnStartup(StartupEventArgs e)
        {
            try
            {
                NativeMethods.theData = (BackendDataViewModel)(this.Resources["eIDViewerBackendObj"]);
                NativeMethods.Init();

                backendThread = new Thread(NativeMethods.backendMainloop);
                backendThread.IsBackground = true;
                backendThread.Start();
                Console.WriteLine("backendThread started");

                base.OnStartup(e);
            }
            catch(Exception ex)
            {
                Console.WriteLine("error starting eID Viewer, error message is: " + ex.Message + "\n");
            }
        }

    }
}
