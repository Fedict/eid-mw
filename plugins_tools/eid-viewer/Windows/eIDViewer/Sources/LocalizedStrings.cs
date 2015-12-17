using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace eIDViewer
{
    public class LocalizedStrings : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        public void NotifyPropertyChanged(String propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private static eIDViewer.Resources.ApplicationStringResources _resource1 = new eIDViewer.Resources.ApplicationStringResources();

        public eIDViewer.Resources.ApplicationStringResources resource1
        {
            get { return _resource1; }

            set
            {
                _resource1 = value;
                NotifyPropertyChanged("resource1");
            }
        }
    }
}
