using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace eIDDataSignCheck
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        eIDDataSignCheck.eIDDataSignCheckViewModel theBackendData = (eIDDataSignCheckViewModel)(App.Current.Resources["eIDDataSignCheckBackendObj"]);

        private void GetFileData(out byte[] storedData)
        {
            //Stream myStream = null;
            String filename = null;
            OpenFileDialog myOpenFileDialog = new OpenFileDialog();

            myOpenFileDialog.Filter = "All files (*.*)|*.*";
            myOpenFileDialog.FilterIndex = 1;

            if (myOpenFileDialog.ShowDialog() == true)
            {
                try
                {
                    if ((filename = myOpenFileDialog.FileName) != null)
                    {
                        System.IO.Stream myStream;
                        if ((myStream = myOpenFileDialog.OpenFile()) != null)
                        {
                            using (myStream)
                            {
                                int length = (int)myStream.Length;
                                storedData = new byte[length];
                                int bytesRead = 0;
                                do
                                {
                                    bytesRead = myStream.Read(storedData, 0, length - bytesRead);
                                } while (bytesRead > 0);
                            }
                        }
                        else
                        {
                            storedData = null;
                        }
                    }
                    else
                    {
                        storedData = null;
                    }
                }
                catch (Exception ex)
                {
                    storedData = null;
                    MessageBox.Show("Error: Could not read file from disk. Error message: " + ex.Message);
                }
            }
            else
            {
                storedData = null;
            }
        }
        private void ID_Click(object sender, RoutedEventArgs e)
        {
            GetFileData(out theBackendData.dataFile);
        }

        private void IDSign_Click(object sender, RoutedEventArgs e)
        {
            GetFileData(out theBackendData.dataSignFile);
        }

        private void Address_Click(object sender, RoutedEventArgs e)
        {
            GetFileData(out theBackendData.addressFile);
        }

        private void AddressSign_Click(object sender, RoutedEventArgs e)
        {
            GetFileData(out theBackendData.addressSignFile);
        }

        private void RNCert_Click(object sender, RoutedEventArgs e)
        {
            GetFileData(out theBackendData.rnCertFile);
        }

        private void Photo_Click(object sender, RoutedEventArgs e)
        {
            try {
                GetFileData(out theBackendData.photoFile);

                if (theBackendData.hashType == "SHA1")
                {
                    SHA1 sha = new SHA1CryptoServiceProvider();
                    theBackendData.photo_hash = sha.ComputeHash(theBackendData.photoFile);
                }
                else if (theBackendData.hashType == "SHA256")
                {
                    SHA256 sha = new SHA256CryptoServiceProvider();
                    theBackendData.photo_hash = sha.ComputeHash(theBackendData.photoFile);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: Could not read file from disk. Error message: " + ex.Message);
            }
        }

        private void VerifyAll_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                theBackendData.VerifyAll();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: Could not read file from disk. Error message: " + ex.Message);
            }
        }

        void HashType_OnSelectionChanged(object sender, RoutedEventArgs e)
        {
            ComboBox logCombo = sender as ComboBox;
            if (logCombo != null)
            {
                switch (logCombo.SelectedIndex)
                {
                    case 0:
                        theBackendData.hashType = "SHA1";
                        if (theBackendData.photoFile != null)
                        {
                            SHA1 sha = new SHA1CryptoServiceProvider();
                            theBackendData.photo_hash = sha.ComputeHash(theBackendData.photoFile);
                        }
                        break;
                    case 1:
                    default:
                        theBackendData.hashType = "SHA256";
                        if (theBackendData.photoFile != null)
                        {
                            SHA256 sha = new SHA256CryptoServiceProvider();
                            theBackendData.photo_hash = sha.ComputeHash(theBackendData.photoFile);
                        }
                        break;
                }
            }
        }

    }
}
