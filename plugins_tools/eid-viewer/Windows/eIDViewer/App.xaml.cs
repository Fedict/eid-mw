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
        protected override void OnStartup(StartupEventArgs e)
        {
            try
            {
                NativeMethods.theData = (BackendDataViewModel)(this.Resources["eIDViewerBackendObj"]);
                NativeMethods.Init();

                base.OnStartup(e);
            }
            catch(Exception ex)
            {
                Console.WriteLine("error starting eID Viewer, error message is: " + ex.Message + "\n");
            }
        }

    }
}
