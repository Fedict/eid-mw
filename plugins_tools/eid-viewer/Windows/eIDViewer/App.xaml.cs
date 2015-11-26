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
        public eIDViewerBackend m_eIDViewerBackend;

        protected override void OnStartup(StartupEventArgs e)
        {
            m_eIDViewerBackend = new eIDViewerBackend();
            m_eIDViewerBackend.Init();

            System.Threading.Thread.Sleep(1000);

            //Wait for EVENT_SET_CALLBACKS set event
            Thread backendThread = new Thread(m_eIDViewerBackend.backendMainloop);
            backendThread.Start();
            Console.WriteLine("backendThread started");

            base.OnStartup(e);
        }

    }
}
