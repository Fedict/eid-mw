using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace eIDDataSignCheck
{
    public partial class eIDDataSignCheckViewModel : INotifyPropertyChanged
    {

        private readonly SynchronizationContext _syncContext;

        public eIDDataSignCheckViewModel()
        {
            // we assume this ctor is called from the UI thread!
            _syncContext = SynchronizationContext.Current;
        }

        ~eIDDataSignCheckViewModel()
        {
            //store application settings
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public void NotifyPropertyChanged(String propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        public byte[] dataFile;
        public byte[] addressFile;
        public byte[] dataSignFile;
        public byte[] addressSignFile;
        public byte[] photoFile;
        public byte[] photo_hash;
        public byte[] rnCertFile;

        private bool _allSignaturesValid;
        public bool allSignaturesValid
        {
            get { return _allSignaturesValid; }
            set
            {
                _allSignaturesValid = value;
                this.NotifyPropertyChanged("allSignaturesValid");
            }
        }

        public string hashType = "SHA256";

}
}

