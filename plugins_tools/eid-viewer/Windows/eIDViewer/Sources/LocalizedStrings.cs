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

        private static eIDViewer.Resources.IdentityTabStringResources _identityTabResource = new eIDViewer.Resources.IdentityTabStringResources();

        public eIDViewer.Resources.IdentityTabStringResources identityTabResource
        {
            get { return _identityTabResource; }

            set
            {
                _identityTabResource = value;
                NotifyPropertyChanged("identityTabResource");
            }
        }

        private static eIDViewer.Resources.CardTabStringResources _cardTabResource = new eIDViewer.Resources.CardTabStringResources();

        public eIDViewer.Resources.CardTabStringResources cardTabResource
        {
            get { return _cardTabResource; }

            set
            {
                _cardTabResource = value;
                NotifyPropertyChanged("cardTabResource");
            }
        }
        private static eIDViewer.Resources.CertificateTabStringResources _certificateTabResource = new eIDViewer.Resources.CertificateTabStringResources();

        public eIDViewer.Resources.CertificateTabStringResources certificateTabResource
        {
            get { return _certificateTabResource; }

            set
            {
                _certificateTabResource = value;
                NotifyPropertyChanged("certificateTabResource");
            }
        }

        private static eIDViewer.Resources.ApplicationStringResources _applicationResource = new eIDViewer.Resources.ApplicationStringResources();

        public eIDViewer.Resources.ApplicationStringResources applicationResource
        {
            get { return _applicationResource; }

            set
            {
                _applicationResource = value;
                NotifyPropertyChanged("applicationResource");
            }
        }

        private static eIDViewer.Resources.MenuItemStringResources _menuItemResource = new eIDViewer.Resources.MenuItemStringResources();

        public eIDViewer.Resources.MenuItemStringResources menuItemResource
        {
            get { return _menuItemResource; }

            set
            {
                _menuItemResource = value;
                NotifyPropertyChanged("menuItemResource");
            }
        }
    }
}
