using System;
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace eIDViewer
{

    public class CertViewModel : INotifyPropertyChanged
    {
        public CertViewModel()
        {
            Certs = new ObservableCollection<CertViewModel>();
            IsExpanded = true;
            CertVisibility = System.Windows.Visibility.Hidden;
            ImagePath = "Resources/CertificateImages/certificate_large.png";
        }

        public void CertNotifyPropertyChanged(String propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        protected virtual void OnCertificateSelectionChanged(EventArgs e)
        {
            EventHandler handler = CertificateSelectionChanged;
            if (handler != null)
            {
                handler(this, e);
            }
        }

        //notify BackendDataViewModule that a new certificate is selected
        public event EventHandler CertificateSelectionChanged;

        //notify the view that one of our properties changed
        public event PropertyChangedEventHandler PropertyChanged;

        //the certificate label, as shown in the treeview
        public string CertLabel { get; set; }
        //the certificate image, as shown in the treeview
        //public string ImagePath { get; set; }
        public string CertSubject { get; set; }
        //public string CertC { get; set; }
        public string CertValidfrom { get; set; }
        public string CertValidUntill { get; set; }
        public string CertUsage { get; set; }
        //the certificate trust status
        public string CertTrust { get; set; }
        public ObservableCollection<CertViewModel> Certs { get; set; }
        public bool IsExpanded { get; set; }

        private System.Windows.Visibility _CertVisibility;
        public System.Windows.Visibility CertVisibility
        {
            get { return _CertVisibility; }
            set
            {
                _CertVisibility = value;
                this.CertNotifyPropertyChanged("CertVisibility");
            }
        }

        private string _ImagePath;
        public string ImagePath
        {
            get { return _ImagePath; }
            set
            {
                _ImagePath = value;
                this.CertNotifyPropertyChanged("ImagePath");
            }
        }

        private bool _isSelected;
        public bool IsSelected
        {
            get { return _isSelected; }
            set
            {
                if (_isSelected != value)
                {
                    _isSelected = value;
                    if (_isSelected)
                    {
                        EventArgs e = new EventArgs();
                        OnCertificateSelectionChanged(e);
                    }
                }
            }
        }

    }
}
