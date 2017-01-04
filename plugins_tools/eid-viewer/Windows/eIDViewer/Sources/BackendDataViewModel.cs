﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Windows.Media.Imaging;
using System.IO;
using System.Windows;
using System.Resources;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using System.Collections.ObjectModel;
using System.Collections.Concurrent;
using System.Reflection;

namespace eIDViewer
{


    public class BackendDataViewModel : INotifyPropertyChanged
    {
        //hashAlg being "SHA1", or "SHA256"
        public bool CheckRNSignature(byte[] data, byte[] signedHash, string hashAlg)
        {
            byte[] HashValue;
            try
            {
                if (hashAlg.Equals("SHA1"))
                {
                    SHA1 sha = new SHA1CryptoServiceProvider();
                    HashValue = sha.ComputeHash(data);
                }
                else if (hashAlg.Equals("SHA256"))
                {
                    SHA256 sha = new SHA256CryptoServiceProvider();
                    HashValue = sha.ComputeHash(data);
                }
                else
                {
                    return false;
                }

                RSACryptoServiceProvider csp = RN_cert.PublicKey.Key as RSACryptoServiceProvider;

                if (csp.VerifyHash(HashValue, CryptoConfig.MapNameToOID(hashAlg), signedHash))
                {
                    Console.WriteLine("The signature is valid.");
                    this.logText += "The signature of the data is valid \n";
                    return true;
                }
                else
                {
                    Console.WriteLine("The signature is not valid.");
                    this.logText += "The signature of the data is not valid \n";
                    ResetDataValues();
                }
            }
            catch (Exception e)
            {
                this.logText += "An error occurred validating the data signature\n Exception message is: " + e.Message + "\n";
            }
            return false;
        }

        public bool CheckShaHash(byte[] data, byte[] shaHash)
        {
            byte[] HashValue;
            try
            {
                if(shaHash.Length == 20)
                {
                    SHA1 sha = new SHA1CryptoServiceProvider();
                    HashValue = sha.ComputeHash(data);
                }
                else if (shaHash.Length == 32)
                {
                    SHA256 sha = new SHA256CryptoServiceProvider();
                    HashValue = sha.ComputeHash(data);
                }
                else
                {
                    return false;
                }
                    
                if (HashValue.Length != shaHash.Length)
                    return false;

                for (int i = 0; i < shaHash.Length; i++)
                {
                    if (HashValue[i] != shaHash[i])
                    {
                        return false;
                    }
                }

                return true;


            }
            catch (Exception e)
            {
                this.logText += "An error occurred computing a sha1 hash \n";
                this.logText += "Exception message: " + e.Message + "\n";
            }
            return false;

        }

        //verify if the chain that was build by .NET is identical (or witch cross-signed root) to the one on the eID Card
        public bool CheckChain(ref X509Chain buildChain, ref X509Certificate2 leafCertificate)
        {
            int chainLen = 3;

            if(leafCertificate.Equals(RN_cert))
            {
                chainLen = 2;
            }
            //chain length should be 3
            //allow cross-signed belgium rootCA
            //if (buildChain.ChainElements.Count != chainLen)
            //{
            //    this.logText += "certificate chain is bigger then 3, did the webtrusted Belgian rootCA entered the chain? \n";
            //    return false;
            //}
            //check if entire chain .NET just build is the one on the eID Card
            if (buildChain.ChainElements[0].Certificate.Thumbprint != leafCertificate.Thumbprint)
            {
                //leaf cert in the verified chain is not the one on the eID Card
                this.logText += "certificate chain not build correctly, leafCertificate in Windows store differs from the one on eID card \n";
               return false;
            }
            if (chainLen == 2)
            {
                if (!buildChain.ChainElements[1].Certificate.GetPublicKey().SequenceEqual(rootCA_cert.GetPublicKey()))
                {
                    //root cert in the verified chain has different public key then the one on the eID Card
                    this.logText += "certificate chain not build correctly, RootCA in Windows store has different public key then the one on the eID Card \n";
                    return false;
                }
            }
            //check if entire chain .NET just build is the one on the eID Card
            else
            {
                if (buildChain.ChainElements[1].Certificate.Thumbprint != intermediateCA_cert.Thumbprint)
                {
                    //intermediateCA cert  in the verified chain is not the one on the eID Card
                    this.logText += "certificate chain not build correctly, intermediateCA in Windows store differs from the one on eID card \n";
                    return false;
                }
                //check if root cert in the verified chain has different public key then the one on the eID Card
                if (!buildChain.ChainElements[2].Certificate.GetPublicKey().SequenceEqual(rootCA_cert.GetPublicKey()))
                {
                    //root cert in the verified chain has different public key then the one on the eID Card
                    this.logText += "certificate chain not build correctly, RootCA in Windows store differs from the one on eID card \n";
                    return false;
                }
            }

            return true;
        }

        public bool VerifyRootCAvsEmbeddedRootCA(ref X509Certificate2 rootCertOnCard, string embeddedRootCA)
        {
            bool foundEmbeddedRootCA = false;
            try
            {
                //for debugging, list all embedded resources
                //string[] names = System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceNames();
                //foreach(string name in names)
                //{
                //     this.logText += name.ToString();
                //}

                using (System.IO.Stream fileStream = System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceStream(embeddedRootCA))
                {
                    if (fileStream != null)
                    {
                        var bytes = new byte[fileStream.Length];
                        fileStream.Read(bytes, 0, bytes.Length);
                        X509Certificate2 fileCert = new X509Certificate2(bytes);

                        if (rootCertOnCard.Thumbprint.Equals(fileCert.Thumbprint))
                        {
                            foundEmbeddedRootCA = true;
                        }
                    }
                    else
                    {
                        this.logText += "embeddedRootCA: %s not found\n" + embeddedRootCA;
                    }
                }
            }
            catch (CryptographicException e)
            {
                this.logText += "An error occurred comparing the Belgium rootCA on the card with the ones in the EIDViewer\n" + e.ToString();
            }
            return foundEmbeddedRootCA;
        }


        public bool VerifyRootCA(ref X509Certificate2 rootCertOnCard, ref CertViewModel rootViewModel)
        {
            bool foundEmbeddedRootCA = false;
            string[] embeddedRootCAs = { "eIDViewer.Resources.Certs.belgiumrca2.pem",
                                         "eIDViewer.Resources.Certs.belgiumrca3.pem",
                                         "eIDViewer.Resources.Certs.belgiumrca4.pem"};

            if( (rootCertOnCard == null) || (rootViewModel == null) )
            {
                this.logText += "No root certificate present to verify\n";
                return false;
            }

            foreach(string embeddedRootCA in embeddedRootCAs)
            {
                if (VerifyRootCAvsEmbeddedRootCA(ref rootCertOnCard, embeddedRootCA) == true)
                {
                    foundEmbeddedRootCA = true;
                    break;
                }
            }

            //if the rootca is not found in our embedded resources, show it as invalid
            if (foundEmbeddedRootCA == false)
            {
                rootViewModel.CertTrust = "Unknow RootCA";
                SetCertificateIcon(rootViewModel, eid_cert_status.EID_CERT_STATUS_INVALID);
            }
            else
            {
                SetCertificateIcon(rootViewModel, eid_cert_status.EID_CERT_STATUS_VALID);
                rootViewModel.CertTrust = "Trusted";
            }

            return foundEmbeddedRootCA;
        }

        public void LogChainInfo(ref X509Chain chain)
        {
            this.logText += "Chain Information \n";
            this.logText += "Chain revocation flag: " + chain.ChainPolicy.RevocationFlag  + "\n";
            this.logText += "Chain revocation mode: " + chain.ChainPolicy.RevocationMode + "\n";
            this.logText += "Chain verification flag: " + chain.ChainPolicy.VerificationFlags + "\n";
            this.logText += "Chain verification time: " + chain.ChainPolicy.VerificationTime + "\n";
            this.logText += "Chain status length: " + chain.ChainStatus.Length + "\n";
            this.logText += "Chain application policy count: " + chain.ChainPolicy.ApplicationPolicy.Count + "\n";
            this.logText += "Chain certificate policy count: " + chain.ChainPolicy.CertificatePolicy.Count + "\n";

            //Output chain element information.
            this.logText += "Chain Element Information \n";
            this.logText += "Number of chain elements: " + chain.ChainElements.Count + "\n";
            this.logText += "Chain elements synchronized? " + chain.ChainElements.IsSynchronized + "\n";
        }

        public void LogChainElement(ref X509ChainElement element, int index)
        {
            this.logText += "Certificate in chain \n";
            this.logText += "subject: " + element.Certificate.Subject + "\n";
            this.logText += "issuer name: " + element.Certificate.Issuer + "\n";
            this.logText += "valid from: " + element.Certificate.NotBefore + "\n";
            this.logText += "valid until: " + element.Certificate.NotAfter + "\n";
        }

        //if .NET cannot build a validated chain, return invalid
        //if the root cert is not know by this application, return invalid
        //if the above are true, but the chain has status information on certificates in the chain, return warning
        //if something went wrong, return unknown
        //if none of the above, return valid
        public void CheckCertificateValidity(ref X509Certificate2 leafCertificate, ref CertViewModel leafCertificateViewModel )
        {
            var chain = new X509Chain();
            eid_cert_status chainStatus = eid_cert_status.EID_CERT_STATUS_UNKNOWN;

            try
            {     
                if (leafCertificate != null)
                {                  
                    //alter how the chain is built/validated.
                    chain.ChainPolicy.RevocationMode = X509RevocationMode.Online;
                    chain.ChainPolicy.RevocationFlag = X509RevocationFlag.ExcludeRoot;
                    // Set the time span that may elapse during online revocation verification or downloading the certificate revocation list (CRL).
                    TimeSpan verificationTime = new TimeSpan(0, 0, 10);
                    chain.ChainPolicy.UrlRetrievalTimeout = verificationTime;
           
                    //add the intermediate and root certs in case they are not already in the windows certificate store
                    chain.ChainPolicy.ExtraStore.Add(intermediateCA_cert);
                    chain.ChainPolicy.ExtraStore.Add(rootCA_cert);

                    leafCertificateViewModel.CertTrust = "Validating..\n";
                    UpdateUICertDetails();
                    // Do the validation
                    bool chainok = chain.Build(leafCertificate);

                    if (chainok == false)
                    {
                        //no valid chain could be build
                        this.logText += "no valid certificate chain could be constructed \n";
                        chainStatus = eid_cert_status.EID_CERT_STATUS_UNKNOWN;
                    }
                    else
                    {
                        //a valid certificate chain is constructed, now verify if it is the same as the one on the eID Card
                        if (CheckChain(ref chain, ref leafCertificate))
                        {
                            this.logText += "certificate chain build correctly \n";
                            chainStatus = eid_cert_status.EID_CERT_STATUS_VALID;
                        }
                        else
                        {
                            this.logText += "this was not the certificate chain we were looking for \n";
                            //leafCertificateViewModel.CertTrust = "Did not validate\n";
                            SetCertificateIcon(leafCertificateViewModel, eid_cert_status.EID_CERT_STATUS_WARNING);
                            return;
                        }
                    }
                    //output chain information in the log
                    LogChainInfo(ref chain);

                    X509ChainElement element;
                    CertViewModel certModel;
                    eid_cert_status certStatus;

                    //go through the chain's elements 
                    for (int elIndex = 0; elIndex < chain.ChainElements.Count; elIndex++)
                    {
                        certStatus = eid_cert_status.EID_CERT_STATUS_UNKNOWN;
                        element = chain.ChainElements[elIndex];

                        if (element.Certificate.Thumbprint.Equals(leafCertificate.Thumbprint))
                        {
                            certModel = leafCertificateViewModel;
                        }
                        else if (element.Certificate.Thumbprint.Equals(intermediateCA_cert.Thumbprint))
                        {
                            certModel = intermediateCAViewModel;
                        }
                        else if (element.Certificate.Thumbprint.Equals(rootCA_cert.Thumbprint))
                        {
                            //certModel = rootCAViewModel;
                            //no ocsp or crl for root available, and we already verified if rootca is one of the application embedded rootca's
                            break;
                        }
                        else
                        {
                            //we're not interested in anything above the belgian rootCA
                            break;
                        }

                        LogChainElement(ref element, elIndex);
                        certModel.CertTrust = "";

                        if (element.ChainElementStatus.Length > 0)
                        {
                            for (int index = 0; index < element.ChainElementStatus.Length; index++)
                            {
                                Console.WriteLine(element.ChainElementStatus[index].Status);
                                Console.WriteLine(element.ChainElementStatus[index].StatusInformation);
                                this.logText += "certificate status is " + element.ChainElementStatus[index].Status + "\n";
                                this.logText += "certificate status information: " + element.ChainElementStatus[index].StatusInformation + "\n";

                                certModel.CertTrust += element.ChainElementStatus[index].StatusInformation;
                                switch (element.ChainElementStatus[index].Status)
                                {
                                    case X509ChainStatusFlags.NoError:
                                        certStatus = eid_cert_status.EID_CERT_STATUS_VALID;
                                        break;
                                    case X509ChainStatusFlags.RevocationStatusUnknown:
                                        if ( (certStatus != eid_cert_status.EID_CERT_STATUS_INVALID) && (certStatus != eid_cert_status.EID_CERT_STATUS_WARNING) )
                                        {
                                            SetCertificateIcon(certModel, eid_cert_status.EID_CERT_STATUS_UNKNOWN);
                                        }
                                        break;
                                    case X509ChainStatusFlags.NoIssuanceChainPolicy:
                                        if (certStatus != eid_cert_status.EID_CERT_STATUS_INVALID)
                                        {
                                            certStatus = eid_cert_status.EID_CERT_STATUS_WARNING;
                                        }
                                        break;
                                    case X509ChainStatusFlags.NotTimeNested:
                                    case X509ChainStatusFlags.OfflineRevocation:
                                        //ignore these
                                        break;
                                    default:
                                        certStatus = eid_cert_status.EID_CERT_STATUS_INVALID;
                                        break;
                                }
                            }
                        }
                        else
                        {
                            //no detailed chain element certificate status, use the chainStatus
                            //e.g. for RN cert, as it has no crl or ocsp
                            certStatus = chainStatus;
                        }
                        SetCertificateIcon(certModel, certStatus);
                        if(certStatus == eid_cert_status.EID_CERT_STATUS_VALID)
                        {
                            certModel.CertTrust = "Trusted";
                        }
                    }
                }
                else
                {
                    this.logText += "Leaf certificate was null, cannot verify \n";
                }
            }

            catch (Exception e)
            {
                this.logText += "An error occurred checking the certificate status \n";
                this.logText += "Exception message: " + e.Message + "\n";
            }     
            finally
            {
                var disposable = chain as IDisposable;
                if (disposable != null)
                {
                    disposable.Dispose();
                }
                UpdateUICertDetails();
            }
            return; 
        }


        public static void TrimEnd(ref byte[] array)
        {
            int lastIndex = Array.FindLastIndex(array, b => b != 0);

            Array.Resize(ref array, lastIndex + 1);
        }

        public void VerifyAllCertificates()
        {
            progress_info = "checking certificates validity";
            if (VerifyRootCA(ref rootCA_cert, ref rootCAViewModel) == true)
            {
                CheckCertificateValidity(ref RN_cert, ref RNCertViewModel);
                CheckCertificateValidity(ref authentication_cert, ref authCertViewModel);
                CheckCertificateValidity(ref signature_cert, ref signCertViewModel);
            }
            else
            {
                this.logText += "this root certificate is not know by this version of the eID Viewer \n";
            }
            progress_info = "";
        }

        public void VerifyAllData()
        {
            if(IsVerifiedDataOK() == false)
            {
                ResetDataValues();
                eIDViewer.NativeMethods.MarkCardInvalid();
                return;
            }
            if (validateAlways == true)
            {
                VerifyAllCertificates();
            }
        }

        public bool IsVerifiedDataOK( )
        {
            string hashAlg;

            if (photo_hash.Length == 20)
            {
                hashAlg = "SHA1";
            }
            else if (photo_hash.Length == 32)
            {
                hashAlg = "SHA256";
            }
            else
            {
                return false;
            }

            //check if the identity signature is ok
            if (CheckRNSignature(dataFile, dataSignFile, hashAlg) != true)
            {
                this.logText += "dataFile signature check failed \n";
                return false;
            }

            //check if the address signature is ok
            byte[] trimmedAddressFile = (byte[]) addressFile.Clone();
            int lastIndex = Array.FindLastIndex(trimmedAddressFile, b => b != 0);
            Array.Resize(ref trimmedAddressFile, lastIndex + 1 + dataSignFile.Length);
            dataSignFile.CopyTo(trimmedAddressFile, lastIndex + 1);

            if (CheckRNSignature(trimmedAddressFile, addressSignFile, hashAlg) != true)
            {
                this.logText += "addressFile signature check failed \n";
                return false;
            }

            //check if the photo corresponds with the photo hash in the identity file
            if (CheckShaHash(photoFile, photo_hash) != true)
            {
                this.logText += "photo doesn't match the hash in the signature file \n";
                return false;
            }

            return true;
        }

        private string _certificateLargeIcon;
        public string certificateLargeIcon
    {
            get { return _certificateLargeIcon; }
            set
            {
                _certificateLargeIcon = value;
                this.NotifyPropertyChanged("certificateLargeIcon");
            }
        }

        public void HideProgressBar()
        {
            progress = 0;
            progress_info = "";
            progress_bar_visible = "Hidden";
        }

        public void SetCertificateIcon(CertViewModel cert, eid_cert_status icon)
        {
            if (cert != null)
            {
                switch (icon)
                {
                    case eid_cert_status.EID_CERT_STATUS_INVALID:
                        cert.ImagePath = "Resources/Images/certificate_bad.png";
                        break;
                    case eid_cert_status.EID_CERT_STATUS_UNKNOWN:
                        cert.ImagePath = "Resources/Images/certificate_large.png";
                        break;
                    case eid_cert_status.EID_CERT_STATUS_VALID:
                        cert.ImagePath = "Resources/Images/certificate_checked.png";
                        break;
                    case eid_cert_status.EID_CERT_STATUS_WARNING:
                        cert.ImagePath = "Resources/Images/certificate_warn.png";
                        break;
                    default:
                        cert.ImagePath = "Resources/Images/certificate_large.png";
                        break;
                }
            }
        }

        public BackendDataViewModel()
        {
            viewerVersion = Assembly.GetExecutingAssembly().GetName().Name + " " + Assembly.GetExecutingAssembly().GetName().Version.ToString();
            validateAlways = Properties.Settings.Default.AlwaysValidate;

            readersList = new ConcurrentQueue<ReadersMenuViewModel>();
            readersList.Enqueue(new ReadersMenuViewModel(" ", 0));

            _certsList = new ObservableCollection<CertViewModel>();
            rootCAViewModel = new CertViewModel { CertLabel = "rootCA" };
            rootCAViewModel.CertificateSelectionChanged += this.CertificateSelectionChanged;
            RNCertViewModel = new CertViewModel { CertLabel = "RN cert" };
            RNCertViewModel.CertificateSelectionChanged += this.CertificateSelectionChanged;
            intermediateCAViewModel = new CertViewModel { CertLabel = "citizen CA" };
            intermediateCAViewModel.CertificateSelectionChanged += this.CertificateSelectionChanged;
            authCertViewModel = new CertViewModel { CertLabel = "Authentication" };
            authCertViewModel.CertificateSelectionChanged += this.CertificateSelectionChanged;
            signCertViewModel = new CertViewModel { CertLabel = "Signature" };
            signCertViewModel.CertificateSelectionChanged += this.CertificateSelectionChanged;

            certsList.Add(rootCAViewModel);

            certsList[0].Certs.Add(RNCertViewModel);
            certsList[0].Certs.Add(intermediateCAViewModel);

            certsList[0].Certs[1].Certs.Add(authCertViewModel);
            certsList[0].Certs[1].Certs.Add(signCertViewModel);
        }

        ~BackendDataViewModel()
        {
            //store application settings
            if(Properties.Settings.Default.AlwaysValidate != validateAlways)
            {
                Properties.Settings.Default.AlwaysValidate = validateAlways;
                Properties.Settings.Default.Save();
            }
        }

        private CertViewModel rootCAViewModel = null;
        private CertViewModel intermediateCAViewModel = null;
        private CertViewModel RNCertViewModel = null;
        private CertViewModel authCertViewModel = null;
        private CertViewModel signCertViewModel = null;

        private X509Certificate2 authentication_cert = null;
        private X509Certificate2 signature_cert = null;
        private X509Certificate2 rootCA_cert = null;
        private X509Certificate2 intermediateCA_cert = null;
        private X509Certificate2 RN_cert = null;

        public event PropertyChangedEventHandler PropertyChanged;

        private CertViewModel currentCertificateView = null;

        //event function notified by CertViewModel when an other certificate is selected
        public void CertificateSelectionChanged(object sender, EventArgs e)
        {
            currentCertificateView = sender as CertViewModel;
            UpdateUICertDetails();
        }

        public void UpdateUICertDetails()
        {
            if (currentCertificateView != null)
            {
                cert_subject = currentCertificateView.CertSubject;
                //cert_c = certificate.CertC;
                cert_valid_from = currentCertificateView.CertValidfrom;
                cert_valid_untill = currentCertificateView.CertValidUntill;
                cert_usage = currentCertificateView.CertUsage;
                cert_trust = currentCertificateView.CertTrust;
                certificateLargeIcon = currentCertificateView.ImagePath;
            }
        }

        private void StoreCertificate (ref CertViewModel theCertViewModel, ref X509Certificate2 theX509Certificate)
        {
            theCertViewModel.CertValidfrom = theX509Certificate.GetEffectiveDateString();
            theCertViewModel.CertValidUntill = theX509Certificate.GetExpirationDateString();
            theCertViewModel.CertVisibility = Visibility.Visible;
            theCertViewModel.CertTrust = "Not checked";
            theCertViewModel.CertSubject = theX509Certificate.Subject.Replace(',', '\n');

            foreach (X509Extension extension in theX509Certificate.Extensions)
            {
                //Console.WriteLine(extension.Oid.FriendlyName + "(" + extension.Oid.Value + ")");
                if (extension.Oid.Value == "2.5.29.15")//OID_KEY_USAGE
                    {
                    theCertViewModel.CertUsage = "";
                    X509KeyUsageExtension ext = (X509KeyUsageExtension)extension;

                    if (ext.KeyUsages.HasFlag(X509KeyUsageFlags.DigitalSignature))
                    {
                        theCertViewModel.CertUsage += "DigitalSignature ";
                    }
                    if (ext.KeyUsages.HasFlag(X509KeyUsageFlags.NonRepudiation))
                    {
                        theCertViewModel.CertUsage += "NonRepudiation ";
                    }
                    if (ext.KeyUsages.HasFlag(X509KeyUsageFlags.KeyCertSign))
                    {
                        theCertViewModel.CertUsage += "KeyCertSign ";
                    }
                    if (ext.KeyUsages.HasFlag(X509KeyUsageFlags.DataEncipherment))
                    {
                        theCertViewModel.CertUsage += "DataEncipherment ";
                    }
                    if (ext.KeyUsages.HasFlag(X509KeyUsageFlags.DecipherOnly))
                    {
                        theCertViewModel.CertUsage += "DecipherOnly ";
                    }
                    if (ext.KeyUsages.HasFlag(X509KeyUsageFlags.EncipherOnly))
                    {
                        theCertViewModel.CertUsage += "EncipherOnly ";
                    }
                    if (ext.KeyUsages.HasFlag(X509KeyUsageFlags.KeyAgreement))
                    {
                        theCertViewModel.CertUsage += "KeyAgreement ";
                    }
                    if (ext.KeyUsages.HasFlag(X509KeyUsageFlags.KeyEncipherment))
                    {
                        theCertViewModel.CertUsage += "KeyEncipherment ";
                    }
                    if (ext.KeyUsages == (X509KeyUsageFlags.None))
                    {
                        theCertViewModel.CertUsage += "No key usage parameters ";
                    }
                }
            }
        }

        private BitmapImage LoadImage(byte[] imageData)
        {
            if (imageData == null || imageData.Length == 0) return null;
            try
            {
                BitmapImage image = new BitmapImage();
                MemoryStream mem = new MemoryStream(imageData);

                mem.Position = 0;
                image.BeginInit();
                image.CreateOptions = BitmapCreateOptions.PreservePixelFormat;
                image.CacheOption = BitmapCacheOption.OnLoad;
                image.UriSource = null;
                image.StreamSource = mem;
                image.EndInit();
                image.Freeze();
                return image;
            }
            catch (Exception e)
            {
                this.logText+= "An error occurred displaying the image \n";
				this.logText += "Exception message: " + e.Message + "\n";
                return null;
            }
        }

        public void StoreStringData(string label, string data)
        {
            progress_info = "reading data";
            progress += 1;
            if (String.Equals(label, "firstnames", StringComparison.Ordinal))
            { firstName = data; }
            else if (String.Equals(label, "surname", StringComparison.Ordinal))
            { surName = data; }
            else if (String.Equals(label, "first_letter_of_third_given_name", StringComparison.Ordinal))
            { first_letter_of_third_given_name = data; }
            else if (String.Equals(label, "date_of_birth", StringComparison.Ordinal))
            { date_of_birth = data; }
            else if (String.Equals(label, "location_of_birth", StringComparison.Ordinal))
            { location_of_birth = data; }
            else if (String.Equals(label, "gender", StringComparison.Ordinal))
            { gender = data; }
            else if (String.Equals(label, "national_number", StringComparison.Ordinal))
            { national_number = data; }
            else if (String.Equals(label, "nationality", StringComparison.Ordinal))
            { nationality = data; }
            else if (String.Equals(label, "nobility", StringComparison.Ordinal))
            { nobility = data; }
            else if (String.Equals(label, "special_status", StringComparison.Ordinal))
            { special_status = data; }
            else if (String.Equals(label, "address_street_and_number", StringComparison.Ordinal))
            { address_street_and_number = data; }
            else if (String.Equals(label, "address_zip", StringComparison.Ordinal))
            { address_zip = data; }
            else if (String.Equals(label, "address_municipality", StringComparison.Ordinal))
            { address_municipality = data; }
            else if (String.Equals(label, "card_number", StringComparison.Ordinal))
            { card_number = data; }
            else if (String.Equals(label, "issuing_municipality", StringComparison.Ordinal))
            { issuing_municipality = data; }
            else if (String.Equals(label, "chip_number", StringComparison.Ordinal))
            { chip_number = data; }
            else if (String.Equals(label, "validity_begin_date", StringComparison.Ordinal))
            { validity_begin_date = data; }
            else if (String.Equals(label, "validity_end_date", StringComparison.Ordinal))
            { validity_end_date = data; }
            else if (String.Equals(label, "document_type", StringComparison.Ordinal))
            { document_type = data; }
            else if (String.Equals(label, "date_and_country_of_protection", StringComparison.Ordinal))
            { date_and_country_of_protection = data;
              foreigner_fields_height = 26;
            }
            else if (String.Equals(label, "member_of_family", StringComparison.Ordinal))
            { member_of_family = true;
              foreigner_fields_height = 26;
            }
        }

        private byte[] dataFile;
        private byte[] addressFile;
        private byte[] dataSignFile;
        private byte[] addressSignFile;
        private byte[] photoFile;
        private byte[] photo_hash;

        public void StoreBinData(string label, byte[] data, int datalen)
        {
            try
            {            
                if (String.Equals(label, "PHOTO_FILE", StringComparison.Ordinal))
                {
                    photoFile = new byte[datalen];
                    data.CopyTo(photoFile, 0);
                    photo = LoadImage(data);
                    progress_info = "reading photo";
                    progress += 4;
                }
                else if (String.Equals(label, "photo_hash", StringComparison.Ordinal))
                {
                    photo_hash = new byte[datalen];
                    data.CopyTo(photo_hash, 0);
                    progress_info = "reading the identity data file";
                    progress += 4;
                }
                else if (String.Equals(label, "DATA_FILE", StringComparison.Ordinal))
                {
                   dataFile = new byte[datalen];
                   data.CopyTo(dataFile,0);
                   progress_info = "reading the identity data file";
                   progress += 4;                         
                }
                else if (String.Equals(label, "ADDRESS_FILE", StringComparison.Ordinal))
                {
                    addressFile = new byte[datalen];
                    data.CopyTo(addressFile, 0);
                    progress_info = "reading the address file";
                    progress += 2;
                }
                else if (String.Equals(label, "SIGN_DATA_FILE", StringComparison.Ordinal))
                {
                    dataSignFile = new byte[datalen];
                    data.CopyTo(dataSignFile, 0);
                    progress_info = "reading the signature of the identity file";
                    progress += 1;
                }
                else if (String.Equals(label, "SIGN_ADDRESS_FILE", StringComparison.Ordinal))
                {
                    addressSignFile = new byte[datalen];
                    data.CopyTo(addressSignFile, 0);
                    progress_info = "reading the signature of the address file";
                    progress += 1;
                }
                else if (String.Equals(label, "chip_number", StringComparison.Ordinal))
                { //chip_number = BitConverter.ToString(data);
                    progress_info = "reading chip_number";
                    chip_number = String.Concat(Array.ConvertAll(data, x => x.ToString("X2")));
                    progress += 1;
                }
                else if (String.Equals(label, "Authentication", StringComparison.Ordinal))
                {
                    progress_info = "reading authentication certificate";
                    authentication_cert = new X509Certificate2(data);
                    StoreCertificate( ref authCertViewModel, ref authentication_cert);
                    progress += 6;
                }
                else if (String.Equals(label, "Signature", StringComparison.Ordinal))
                {
                    progress_info = "reading signature certificate";
                    signature_cert = new X509Certificate2(data);
                    StoreCertificate(ref signCertViewModel, ref signature_cert);
                    progress += 6;
                }
                else if (String.Equals(label, "Root", StringComparison.Ordinal))
                {
                    progress_info = "reading root certificate";
                    rootCA_cert = new X509Certificate2(data);
                    StoreCertificate(ref rootCAViewModel, ref rootCA_cert);
                    progress += 6;
                }
                else if (String.Equals(label, "CA", StringComparison.Ordinal))
                {
                    progress_info = "reading intermediate certificate";
                    intermediateCA_cert = new X509Certificate2(data);
                    StoreCertificate(ref intermediateCAViewModel, ref intermediateCA_cert);
                    progress += 6;
                }
                else if (String.Equals(label, "CERT_RN_FILE", StringComparison.Ordinal))
                {
                    progress_info = "reading RN certificate";
                    RN_cert = new X509Certificate2(data);
                    StoreCertificate(ref RNCertViewModel, ref RN_cert);
                    progress += 6;
                }
            }
            catch (Exception e)
            {
                this.logText += "An error occurred storing binary data of " + label + "\n";
                this.logText += "Exception message: " + e.Message + "\n";
            }
        }

        public void SetLanguage (eid_vwr_langs language)
        {
            switch (language)
            {
                case eid_vwr_langs.EID_VWR_LANG_DE:
                    languageDE = true;
                    languageEN = false;
                    languageFR = false;
                    languageNL = false;
                    break;
                case eid_vwr_langs.EID_VWR_LANG_EN:
                    languageDE = false;
                    languageEN = true;
                    languageFR = false;
                    languageNL = false;
                    break;
                case eid_vwr_langs.EID_VWR_LANG_FR:
                    languageDE = false;
                    languageEN = false;
                    languageFR = true;
                    languageNL = false;
                    break;
                case eid_vwr_langs.EID_VWR_LANG_NL:
                    languageDE = false;
                    languageEN = false;
                    languageFR = false;
                    languageNL = true;
                    break;
                default:
                    languageDE = false;
                    languageEN = false;
                    languageFR = false;
                    languageNL = false;
                    break;


            }
        }


        public void ResetDataValues()
        {
            text_color = "Gray";
            document_type = "-";
            _firstName = "";
            _first_letter_of_third_given_name = "";
            firstNames = "-";
            surName = "-";
            date_of_birth = "-";
            location_of_birth = "-";
            gender = "-";
            national_number = "-";
            nationality = "-";
            nobility = "-";
            special_status = "-";
            address_street_and_number = "-";
            address_zip = "-";
            address_municipality = "-";
            card_number = "-";
            issuing_municipality = "-";
            chip_number = "-";
            validity_begin_date = "-";
            validity_end_date = "-";
            date_and_country_of_protection = "-";
            member_of_family = false;
            eid_card_present = false;
            progress = 0;
            HideProgressBar();
            foreigner_fields_height = 0;
            pinop_ready = false;
            print_enabled = false;

            photo = null;

            rootCAViewModel.ClearData();
            intermediateCAViewModel.ClearData();
            RNCertViewModel.ClearData();
            authCertViewModel.ClearData();
            signCertViewModel.ClearData();

            UpdateUICertDetails();
            authentication_cert = null;
            signature_cert = null;
            rootCA_cert = null;
            intermediateCA_cert = null;
            RN_cert = null; 
        }

        public void NotifyPropertyChanged(String propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private Boolean _languageDE;
        public Boolean languageDE
        {
            get { return _languageDE; }
            set
            {
                _languageDE = value;
                this.NotifyPropertyChanged("languageDE");
            }
        }

        private Boolean _languageNL;
        public Boolean languageNL
        {
            get { return _languageNL; }
            set
            {
                _languageNL = value;
                this.NotifyPropertyChanged("languageNL");
            }
        }

        private Boolean _languageEN;
        public Boolean languageEN
        {
            get { return _languageEN; }
            set
            {
                _languageEN = value;
                this.NotifyPropertyChanged("languageEN");
            }
        }

        private Boolean _languageFR;
        public Boolean languageFR
        {
            get { return _languageFR; }
            set
            {
                _languageFR = value;
                this.NotifyPropertyChanged("languageFR");
            }
        }

        private Boolean _validateAlways;
        public Boolean validateAlways
        {
            get { return _validateAlways; }
            set
            {
                _validateAlways = value;
                this.NotifyPropertyChanged("validateAlways");
            }
        }

        private Boolean _eid_data_ready;
        public Boolean eid_data_ready
        {
            get { return _eid_data_ready; }
            set
            {
                _eid_data_ready = value;
                this.NotifyPropertyChanged("eid_data_ready");
            }
        }

        private Boolean _eid_data_from_file;
        public Boolean eid_data_from_file
        {
            get { return _eid_data_from_file; }
            set
            {
                _eid_data_from_file = value;
                this.NotifyPropertyChanged("eid_data_from_file");
            }
        }

        private string _logtext;
        public string logText
        {
            get { return _logtext; }
            set
            {
                _logtext = value;
                NotifyPropertyChanged("logText");
            }
        }

        private string _firstName;
        public string firstName
        {
            get { return _firstName; }
            set
            {
                _firstName = value;
                _firstNames = value + " " + _first_letter_of_third_given_name;
                this.NotifyPropertyChanged("firstNames");
            }
        }

        private string _surName;
        public string surName
        {
            get { return _surName; }
            set
            {
                _surName = value;
                this.NotifyPropertyChanged("surName");
            }
        }

        private string _first_letter_of_third_given_name;
        public string first_letter_of_third_given_name
        {
            get { return _first_letter_of_third_given_name; }
            set
            {
                _first_letter_of_third_given_name = value;
                _firstNames = _firstName + " " + value;
                this.NotifyPropertyChanged("firstNames");
            }
        }

        private string _firstNames;
        public string firstNames
        {
            get { return _firstNames; }
            set
            {
                _firstNames = value;
                this.NotifyPropertyChanged("firstNames");
            }
        }

        private string _date_of_birth;
        public string date_of_birth
        {
            get { return _date_of_birth; }
            set
            {
                _date_of_birth = value;
                this.NotifyPropertyChanged("date_of_birth");
            }
        }

        private string _location_of_birth;
        public string location_of_birth
        {
            get { return _location_of_birth; }
            set
            {
                _location_of_birth = value;
                this.NotifyPropertyChanged("location_of_birth");
            }
        }

        private string _gender;
        public string gender
        {
            get { return _gender; }
            set
            {
                _gender = value;
                this.NotifyPropertyChanged("gender");
            }
        }

        private string _national_number;
        public string national_number
        {
            get { return _national_number; }
            set
            {
                _national_number = value;
                this.NotifyPropertyChanged("national_number");
            }
        }

        private string _nationality;
        public string nationality
        {
            get { return _nationality; }
            set
            {
                _nationality = value;
                this.NotifyPropertyChanged("nationality");
            }
        }

        private string _nobility;
        public string nobility
        {
            get { return _nobility; }
            set
            {
                _nobility = value;
                this.NotifyPropertyChanged("nobility");
            }
        }

        private string _special_status;
        public string special_status
        {
            get { return _special_status; }
            set
            {
                _special_status = value;
                this.NotifyPropertyChanged("special_status");
            }
        }

        private string _address_street_and_number;
        public string address_street_and_number
        {
            get { return _address_street_and_number; }
            set
            {
                _address_street_and_number = value;
                this.NotifyPropertyChanged("address_street_and_number");
            }
        }
        private string _address_zip;
        public string address_zip
        {
            get { return _address_zip; }
            set
            {
                _address_zip = value;
                this.NotifyPropertyChanged("address_zip");
            }
        }
        private string _address_municipality;
        public string address_municipality
        {
            get { return _address_municipality; }
            set
            {
                _address_municipality = value;
                this.NotifyPropertyChanged("address_municipality");
            }
        }

        private string _card_number;
        public string card_number
        {
            get { return _card_number; }
            set
            {
                _card_number = value;
                this.NotifyPropertyChanged("card_number");
            }
        }

        private string _issuing_municipality;
        public string issuing_municipality
        {
            get { return _issuing_municipality; }
            set
            {
                _issuing_municipality = value;
                this.NotifyPropertyChanged("issuing_municipality");
            }
        }

        private string _chip_number;
        public string chip_number
        {
            get { return _chip_number; }
            set
            {
                _chip_number = value;
                this.NotifyPropertyChanged("chip_number");
            }
        }

        private string _validity_begin_date;
        public string validity_begin_date
        {
            get { return _validity_begin_date; }
            set
            {
                _validity_begin_date = value;
                this.NotifyPropertyChanged("validity_begin_date");
            }
        }

        private string _validity_end_date;
        public string validity_end_date
        {
            get { return _validity_end_date; }
            set
            {
                _validity_end_date = value;
                this.NotifyPropertyChanged("validity_end_date");
            }
        }

        private string _date;
        public string date
        {
            get { return _date; }
            set
            {
                _date = value;
                this.NotifyPropertyChanged("date");
            }
        }

        private string _viewerVersion;
        public string viewerVersion
        {
            get { return _viewerVersion; }
            set
            {
                _viewerVersion = value;
                this.NotifyPropertyChanged("viewerVersion");
            }
        }

        private string _document_type;
        public string document_type
        {
            get { return _document_type; }
            set
            {
                _document_type = value;
                this.NotifyPropertyChanged("document_type");
            }
        }

        private string _date_and_country_of_protection;
        public string date_and_country_of_protection
        {
            get { return _date_and_country_of_protection; }
            set
            {
                _date_and_country_of_protection = value;
                this.NotifyPropertyChanged("date_and_country_of_protection");
            }
        }

        private bool _member_of_family;
        public bool member_of_family
        {
            get { return _member_of_family; }
            set
            {
                _member_of_family = value;
                this.NotifyPropertyChanged("member_of_family");
            }
        }

        private int _foreigner_fields_height;
        public int foreigner_fields_height
        {
            get { return _foreigner_fields_height; }
            set
            {
                _foreigner_fields_height = value;
                this.NotifyPropertyChanged("foreigner_fields_height");
            }
        }

        private BitmapImage _photo;
        public BitmapImage photo
        {
            get { return _photo; }
            set
            {
                _photo = value;
                this.NotifyPropertyChanged("photo");
            }
        }

        private BitmapImage _cardreader_icon;
        public BitmapImage cardreader_icon
        {
            get { return _cardreader_icon; }
            set
            {
                _cardreader_icon = value;
                this.NotifyPropertyChanged("cardreader_icon");
            }
        }

        private ObservableCollection<CertViewModel> _certsList;
        public ObservableCollection<CertViewModel> certsList
        {
            get { return _certsList; }
            set
            {
                _certsList = value;
                this.NotifyPropertyChanged("certsList");
            }
        }
        //TDO : not thread safe atm
        private ConcurrentQueue<ReadersMenuViewModel> _readersList;
        public ConcurrentQueue<ReadersMenuViewModel> readersList
        {
            get { return _readersList; }
            set
            {
                _readersList = value;
                this.NotifyPropertyChanged("readersList");
            }
        }

        private string _cert_subject;
        public string cert_subject
        {
            get { return _cert_subject; }
            set
            {
                _cert_subject = value;
                this.NotifyPropertyChanged("cert_subject");
            }
        }

        private string _cert_valid_from;
        public string cert_valid_from
        {
            get { return _cert_valid_from; }
            set
            {
                _cert_valid_from = value;
                this.NotifyPropertyChanged("cert_valid_from");
            }
        }

        private string _cert_valid_untill;
        public string cert_valid_untill
        {
            get { return _cert_valid_untill; }
            set
            {
                _cert_valid_untill = value;
                this.NotifyPropertyChanged("cert_valid_untill");
            }
        }

        private string _cert_usage;
        public string cert_usage
        {
            get { return _cert_usage; }
            set
            {
                _cert_usage = value;
                this.NotifyPropertyChanged("cert_usage");
            }
        }

        private string _cert_trust;
        public string cert_trust
        {
            get { return _cert_trust; }
            set
            {
                _cert_trust = value;
                this.NotifyPropertyChanged("cert_trust");
            }
        }

        private String _text_color;
        public String text_color
        {
            get { return _text_color; }
            set
            {
                _text_color = value;
                this.NotifyPropertyChanged("text_color");
            }
        }

        private Boolean _eid_card_present;
        public Boolean eid_card_present
        {
            get { return _eid_card_present; }
            set
            {
                _eid_card_present = value;
                this.NotifyPropertyChanged("eid_card_present");
            }
        }

        private int _progress;
        public int progress
        {
            get { return _progress; }
            set
            {
                _progress = value;
                this.NotifyPropertyChanged("progress");
                //if (progress >= 100)
                //{
                //    progress_bar_visible = "Hidden";
                //}
            }
        }
        private String _progress_bar_visible = "Hidden";
        public String progress_bar_visible
        {
            get { return _progress_bar_visible; }
            set
            {
                _progress_bar_visible = value;
                this.NotifyPropertyChanged("progress_bar_visible");
            }
        }
        private String _progress_info = "";
        public String progress_info
        {
            get { return _progress_info; }
            set
            {
                _progress_info = value;
                this.NotifyPropertyChanged("progress_info");
            }
        }

        private eid_vwr_loglevel _log_level = eid_vwr_loglevel.EID_VWR_LOG_DETAIL;
        public eid_vwr_loglevel log_level
        {
            get { return _log_level; }
            set
            {
                _log_level = value;
                this.NotifyPropertyChanged("log_level");
            }
        }

        private Boolean _pinop_ready = false;
        public Boolean pinop_ready
        {
            get { return _pinop_ready; }
            set
            {
                _pinop_ready = value;
                this.NotifyPropertyChanged("pinop_ready");
            }
        }

        private Boolean _print_enabled = false;
        public Boolean print_enabled
        {
            get { return _print_enabled; }
            set
            {
                _print_enabled = value;
                this.NotifyPropertyChanged("print_enabled");
            }
        }

    }
}
