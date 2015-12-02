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
        //public eIDViewerBackend m_eIDViewerBackend;
        private Thread backendThread;

        protected override void OnStartup(StartupEventArgs e)
        {
            //eIDViewerBackend theeIDViewerBackendObj = (eIDViewerBackend)App.Current.Resources["eIDViewerBackendObj"];

            ((eIDViewerBackend)App.Current.Resources["eIDViewerBackendObj"]).Init();

           // System.Threading.Thread.Sleep(10000);

            //Wait for EVENT_SET_CALLBACKS set event
            backendThread = new Thread(((eIDViewerBackend)App.Current.Resources["eIDViewerBackendObj"]).backendMainloop);
            backendThread.Start();
            Console.WriteLine("backendThread started");

            base.OnStartup(e);
        }

    }
}
