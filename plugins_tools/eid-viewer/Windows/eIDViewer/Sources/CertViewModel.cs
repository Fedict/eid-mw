using System;
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.Windows.Input;
using System.Windows;
using Microsoft.Win32;
using System.IO;
using System.Windows.Controls.Primitives;
using System.Security.Cryptography.X509Certificates;

namespace eIDViewer
{

    public class CertViewModel : INotifyPropertyChanged
    {
        public CertViewModel()
        {
            Certs = new ObservableCollection<CertViewModel>();
            IsExpanded = true;
            CertVisibility = System.Windows.Visibility.Hidden;
            ImagePath = "Resources/Images/certificate_large.png";
            _SaveCommand = null;
        }

        public void ClearData()
        {
            CertVisibility = System.Windows.Visibility.Hidden;
            ImagePath = "Resources/Images/certificate_large.png";
            CertSubject = "";
            CertValidfrom = "";
            CertValidUntill = "";
            CertUsage = "";
            CertTrust = "";
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

        public X509Certificate2 Cert;
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

        private ICommand _DetailInfoCommand;
        public ICommand DetailInfoCommand
        {
            get
            {
                if (_DetailInfoCommand == null)
                {
                    _DetailInfoCommand = new RelayCommand((o) =>
                    {
                        System.Security.Cryptography.X509Certificates.X509Certificate2UI.DisplayCertificate(Cert);
                    });
                }
                return _DetailInfoCommand;
            }
        }

        private ICommand _SaveCommand;
        public ICommand SaveCommand
        {
            get
            {
                if (_SaveCommand == null)
                {
                    _SaveCommand = new RelayCommand((o) =>
                    {
                        String filename = null;
                        
                        SaveFileDialog mySaveFileDialog = new SaveFileDialog();
                        mySaveFileDialog.FileName = CertLabel + ".DER";
                        mySaveFileDialog.Filter = "DER files (*.DER)|*.DER|All files (*.*)|*.*";
                        mySaveFileDialog.FilterIndex = 1;

                        if (mySaveFileDialog.ShowDialog() == true)
                        {
                            try
                            {
                                if ((filename = mySaveFileDialog.FileName) != null)
                                {
                                    using (FileStream output = File.OpenWrite(filename))
                                    {
                                        byte[] CertData = Cert.GetRawCertData();
                                        output.Write(CertData, 0, CertData.Length);
                                    }
                                }
                            }
                            catch (Exception ex)
                            {
                                MessageBox.Show("Error: Could not save file to disk. Error message: " + ex.Message);
                            }
                        }
                    });
                }
                return _SaveCommand;
            }
        }

        public class RelayCommand : ICommand
        {
            #region Fields
            readonly Action<object> _execute;
            readonly Predicate<object> _canExecute;
            #endregion // Fields

            #region Constructors
            public RelayCommand(Action<object> execute)
                : this(execute, null)
            {
            }
            public RelayCommand(Action<object> execute, Predicate<object> canExecute)
            {
                if (execute == null)
                    throw new ArgumentNullException("execute");

                _execute = execute;
                _canExecute = canExecute;
            }
            #endregion // Constructors
            #region ICommand Members
            public bool CanExecute(object parameter)
            {
                return _canExecute == null ? true : _canExecute(parameter);
            }
            public event EventHandler CanExecuteChanged
            {
                add { CommandManager.RequerySuggested += value; }
                remove { CommandManager.RequerySuggested -= value; }
            }
            public void Execute(object parameter)
            {
                _execute(parameter);
            }

            #endregion // ICommand Members
        }
    }

}

