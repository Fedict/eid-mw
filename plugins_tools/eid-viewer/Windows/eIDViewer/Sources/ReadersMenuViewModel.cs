using System;
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace eIDViewer
{

    public class ReadersMenuViewModel : INotifyPropertyChanged
    {
        public ReadersMenuViewModel(string theReaderName, UInt32 theSlotNumber)
        {
            readerName = theReaderName;
            slotNumber = theSlotNumber;
        }

        public void ReadersMenuNotifyPropertyChanged(String propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        //notify the view that one of our properties changed
        public event PropertyChangedEventHandler PropertyChanged;

        private string _readerName;
        public string readerName
        {
            get { return _readerName; }
            set
            {
                _readerName = value;
                this.ReadersMenuNotifyPropertyChanged("readerName");
            }
        }
        public UInt32 slotNumber;

    }
}
