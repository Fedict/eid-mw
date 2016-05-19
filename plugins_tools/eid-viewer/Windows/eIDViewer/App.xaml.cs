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
            NativeMethods.theData = (BackendDataViewModel)(this.Resources["eIDViewerBackendObj"]);
            NativeMethods.Init();

            backendThread = new Thread(NativeMethods.backendMainloop);
            backendThread.Start();
            Console.WriteLine("backendThread started");

            base.OnStartup(e);
        }
        /*
        protected override void OnExit(ExitEventArgs e)
        {
            //eIDViewerBackend.CSCblog(0, "On Exit");
            base.OnExit(e);
        }*/

    }
}
