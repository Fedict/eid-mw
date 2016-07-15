using System;
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
                this.logText += "An error occurred validating the data signature \n";
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
            }
            return false;

        }

        static bool VerifyCertificate(byte[] primaryCertificate, IEnumerable<byte[]> additionalCertificates)
        {
            var chain = new X509Chain();
            foreach (var cert in additionalCertificates.Select(x => new X509Certificate2(x)))
            {
                chain.ChainPolicy.ExtraStore.Add(cert);
            }

            // You can alter how the chain is built/validated.
            chain.ChainPolicy.RevocationMode = X509RevocationMode.Online;
            chain.ChainPolicy.VerificationFlags = X509VerificationFlags.NoFlag;

            // Do the validation.
            var primaryCert = new X509Certificate2(primaryCertificate);
            return chain.Build(primaryCert);
        }

        //verify if the chain that was build by .NET is identical to the one on the eID Card
        public bool CheckChain(ref X509Chain buildChain, ref X509Certificate2 leafCertificate)
        {
            bool chainIsOnEIDCard = true;
            //chain length should be 3
            if (buildChain.ChainElements.Count != 3)
            {
                this.logText += "certificate chain is bigger then 3, did the webtrusted Belgian rootCA entered the chain? \n";
                chainIsOnEIDCard = false;
            }
            //check if entire chain .NET just build is the one on the eID Card
            else if (buildChain.ChainElements[0].Certificate.Thumbprint != leafCertificate.Thumbprint)
            {
                //root cert in the verified chain is not the one on the eID Card
                this.logText += "certificate chain not build correctly, RootCA in Windows store differs from the one on eID card \n";
                chainIsOnEIDCard = false;
            }
            //check if entire chain .NET just build is the one on the eID Card
            else if (buildChain.ChainElements[1].Certificate.Thumbprint != intermediateCA_cert.Thumbprint)
            {
                //root cert in the verified chain is not the one on the eID Card
                this.logText += "certificate chain not build correctly, RootCA in Windows store differs from the one on eID card \n";
                chainIsOnEIDCard = false;
            }
            //check if entire chain .NET just build is the one on the eID Card
            else if (buildChain.ChainElements[2].Certificate.Thumbprint != rootCA_cert.Thumbprint)
            {
                //root cert in the verified chain is not the one on the eID Card
                this.logText += "certificate chain not build correctly, RootCA in Windows store differs from the one on eID card \n";
                chainIsOnEIDCard = false;
            }

            return chainIsOnEIDCard;
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
                    var bytes = new byte[fileStream.Length];
                    fileStream.Read(bytes, 0, bytes.Length);
                    X509Certificate2 fileCert = new X509Certificate2(bytes);

                    if (rootCertOnCard.Thumbprint.Equals(fileCert.Thumbprint))
                    {
                        foundEmbeddedRootCA = true;
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
            string[] embeddedRootCAs = { "eIDViewer.Resources.Certs.belgiumrca.pem",
                                         "eIDViewer.Resources.Certs.belgiumrca2.pem",
                                         "eIDViewer.Resources.Certs.belgiumrca3.pem",
                                         "eIDViewer.Resources.Certs.belgiumrca4.pem"};

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
            this.logText += "is valid: " + element.Certificate.Verify() + "\n";
        }

        //if .NET cannot build a validated chain, return invalid
        //if the root cert is not know by this application, return invalid
        //if the above are true, but the chain has status information on certificates in the chain, return warning
        //if something went wrong, return unknown
        //if none of the above, return valid
        public eid_cert_status CheckCertificateValidity(ref X509Certificate2 leafCertificate, ref CertViewModel leafCertificateViewModel )
        {
            var chain = new X509Chain();
            eid_cert_status chainStatus = eid_cert_status.EID_CERT_STATUS_UNKNOWN;
            try
            {     
                if (leafCertificate != null)
                {                  
                    //alter how the chain is built/validated.
                    chain.ChainPolicy.RevocationMode = X509RevocationMode.Online;
                    //chain.ChainPolicy.RevocationFlag = X509RevocationFlag.ExcludeRoot;
                    // Set the time span that may elapse during online revocation verification or downloading the certificate revocation list (CRL).
                    TimeSpan verificationTime = new TimeSpan(0, 0, 20);
                    chain.ChainPolicy.UrlRetrievalTimeout = verificationTime;
           
                    //add the intermediate and root certs in case they are not already in the windows certificate store
                    chain.ChainPolicy.ExtraStore.Add(intermediateCA_cert);
                    chain.ChainPolicy.ExtraStore.Add(rootCA_cert);

                    // Do the validation
                    bool chainok = chain.Build(leafCertificate);

                    if (chainok == false)
                    {
                        //no valid chain could be build
                        this.logText += "no valid certificate chain could be constructed \n";
                        SetCertificateIcon(leafCertificateViewModel, eid_cert_status.EID_CERT_STATUS_INVALID);
                        chainStatus = eid_cert_status.EID_CERT_STATUS_INVALID;
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
                            return eid_cert_status.EID_CERT_STATUS_WARNING;
                        }
                    }
                    //output chain information in the log
                    LogChainInfo(ref chain);

                    //go through the chain's elements 
                    for (int elIndex = 0; elIndex < chain.ChainElements.Count; elIndex++)
                    {
                        X509ChainElement element = chain.ChainElements[elIndex];
                        CertViewModel certModel;

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
                            certModel = rootCAViewModel;
                        }
                        else
                        {
                            //should never get here, CheckChain() should have seen it
                            break;
                        }

                        LogChainElement(ref element, elIndex);

                        if(element.ChainElementStatus.Length > 0)
                        {
                            if (chainStatus != eid_cert_status.EID_CERT_STATUS_INVALID)
                            {
                                chainStatus = eid_cert_status.EID_CERT_STATUS_WARNING;
                            }
                            for (int index = 0; index < element.ChainElementStatus.Length; index++)
                            {
                                Console.WriteLine(element.ChainElementStatus[index].Status);
                                Console.WriteLine(element.ChainElementStatus[index].StatusInformation);
                                this.logText += "certificate status is " + element.ChainElementStatus[index].Status + "\n";
                                this.logText += "certificate status information: " + element.ChainElementStatus[index].StatusInformation + "\n";

                                certModel.CertTrust = element.ChainElementStatus[index].StatusInformation;
                                if (element.ChainElementStatus[index].Status != X509ChainStatusFlags.NoError)
                                {                          
                                    if (element.ChainElementStatus[index].Status == X509ChainStatusFlags.RevocationStatusUnknown)
                                    {
                                        SetCertificateIcon(certModel, eid_cert_status.EID_CERT_STATUS_UNKNOWN);
                                    }
                                    else
                                    {
                                        SetCertificateIcon(certModel, eid_cert_status.EID_CERT_STATUS_INVALID);
                                    }
                                }
                                else
                                {
                                    // X509ChainStatusFlags.NoError
                                    SetCertificateIcon(certModel, eid_cert_status.EID_CERT_STATUS_VALID);
                                }
                            }
                        }
                        else
                        {
                            //no chain status, everything is fine
                            SetCertificateIcon(certModel, eid_cert_status.EID_CERT_STATUS_VALID);
                        }
                    }
                }
            }

            catch (Exception e)
            {
                this.logText += "An error occurred checking the certificate status \n";
            }     
            finally
            {
                var disposable = chain as IDisposable;
                if (disposable != null)
                {
                    disposable.Dispose();
                }            
            }
            return chainStatus; 
        }


        public static void TrimEnd(ref byte[] array)
        {
            int lastIndex = Array.FindLastIndex(array, b => b != 0);

            Array.Resize(ref array, lastIndex + 1);
        }

        public void VerifyAllCertificates()
        {
            //TODO: verify RNcert

            progress_info = "checking certificates validity";
            if (VerifyRootCA(ref rootCA_cert, ref rootCAViewModel) == true)
            {
                eid_cert_status authcertStatus = CheckCertificateValidity(ref authentication_cert, ref authCertViewModel);
                SetCertificateLargeIcon(authcertStatus);
                //handle it in UI
                eid_cert_status signcertStatus = CheckCertificateValidity(ref signature_cert, ref signCertViewModel);
                if (signcertStatus > authcertStatus)
                    SetCertificateLargeIcon(signcertStatus);
            }
            else
            {
                this.logText += "this root certificate is not know by this version of the eID Viewer \n";
            }
            progress_info = "";
        }

        public void AllDataRead( )
        {
            string hashAlg = "SHA1";
            //check if the identity signature is ok
            if (CheckRNSignature(dataFile, dataSignFile, hashAlg) != true)
            {
                this.logText += "dataFile signature check failed \n";
                ResetDataValues();
                return;
            }

            //check if the address signature is ok
            byte[] trimmedAddressFile = (byte[]) addressFile.Clone();
            int lastIndex = Array.FindLastIndex(trimmedAddressFile, b => b != 0);
            Array.Resize(ref trimmedAddressFile, lastIndex + 1 + dataSignFile.Length);
            dataSignFile.CopyTo(trimmedAddressFile, lastIndex + 1);

            if (CheckRNSignature(trimmedAddressFile, addressSignFile, hashAlg) != true)
            {
                this.logText += "addressFile signature check failed \n";
                ResetDataValues();
                return;
            }

            //check if the photo corresponds with the photo hash in the identity file
            if (CheckShaHash(photoFile, photo_hash) != true)
            {
                this.logText += "photo doesn't match the hash in the signature file \n";
                ResetDataValues();
                return;
            }

            if (validateAlways == true)
            {
                VerifyAllCertificates();
            }

            HideProgressBar();

            print_enabled = true;
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
            progress = 100;
            progress_info = "";
            progress_bar_visible = "Hidden";
        }

        public void SetCertificateLargeIcon (eid_cert_status icon)
        {
            switch(icon)
            {
                case eid_cert_status.EID_CERT_STATUS_INVALID:
                    certificateLargeIcon = "Resources/CertificateImages/certificate_bad.png";
                    break;
                case eid_cert_status.EID_CERT_STATUS_UNKNOWN:
                    certificateLargeIcon = "Resources/CertificateImages/certificate_large.png";
                    break;
                case eid_cert_status.EID_CERT_STATUS_VALID:
                    certificateLargeIcon = "Resources/CertificateImages/certificate_checked.png";
                    break;
                case eid_cert_status.EID_CERT_STATUS_WARNING:
                    certificateLargeIcon = "Resources/CertificateImages/certificate_warn.png";
                    break;
                default:
                    certificateLargeIcon = "Resources/CertificateImages/certificate_large.png";
                    break;
            }
        }

        public void SetCertificateIcon(CertViewModel cert, eid_cert_status icon)
        {
            switch (icon)
            {
                case eid_cert_status.EID_CERT_STATUS_INVALID:
                    cert.ImagePath = "Resources/CertificateImages/certificate_bad.png";
                    break;
                case eid_cert_status.EID_CERT_STATUS_UNKNOWN:
                    cert.ImagePath = "Resources/CertificateImages/certificate_large.png";
                    break;
                case eid_cert_status.EID_CERT_STATUS_VALID:
                    cert.ImagePath = "Resources/CertificateImages/certificate_checked.png";
                    break;
                case eid_cert_status.EID_CERT_STATUS_WARNING:
                    cert.ImagePath = "Resources/CertificateImages/certificate_warn.png";
                    break;
                default:
                    cert.ImagePath = "Resources/CertificateImages/certificate_large.png";
                    break;
            }
        }


        public BackendDataViewModel()
        {
            validateAlways = Properties.Settings.Default.AlwaysValidate;

            _readersList = new ObservableCollection<ReadersMenuViewModel>();
            //readersList.Add(new ReadersMenuViewModel("No Readers Found", 0));

            SetCertificateLargeIcon(eid_cert_status.EID_CERT_STATUS_UNKNOWN);
            _certsList = new ObservableCollection<CertViewModel>();
            cert_collection = new X509Certificate2Collection();
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

        private CertViewModel rootCAViewModel;
        private CertViewModel intermediateCAViewModel;
        private CertViewModel RNCertViewModel;
        private CertViewModel authCertViewModel;
        private CertViewModel signCertViewModel;

        private X509Certificate2 authentication_cert;
        private X509Certificate2 signature_cert;
        private X509Certificate2 rootCA_cert;
        private X509Certificate2 intermediateCA_cert;
        private X509Certificate2 RN_cert;

        public event PropertyChangedEventHandler PropertyChanged;
        private X509Certificate2Collection cert_collection;

        //event function notified by CertViewModel when an other certificate is selected
        public void CertificateSelectionChanged(object sender, EventArgs e)
        {
            CertViewModel certificate = sender as CertViewModel;
            cert_subject = certificate.CertSubject;
            //cert_c = certificate.CertC;
            cert_valid_from = certificate.CertValidfrom;
            cert_valid_untill = certificate.CertValidUntill;
            cert_usage = certificate.CertUsage;
            cert_trust = certificate.CertTrust;
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
               // Console.WriteLine("An error occurred: '{0}'", e);
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
            { _document_type = data; }
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
                    cert_collection.Add(authentication_cert);
                    StoreCertificate( ref authCertViewModel, ref authentication_cert);
                    progress += 6;
                }
                else if (String.Equals(label, "Signature", StringComparison.Ordinal))
                {
                    progress_info = "reading signature certificate";
                    signature_cert = new X509Certificate2(data);
                    cert_collection.Add(signature_cert);
                    StoreCertificate(ref signCertViewModel, ref signature_cert);
                    progress += 6;
                }
                else if (String.Equals(label, "Root", StringComparison.Ordinal))
                {
                    progress_info = "reading root certificate";
                    rootCA_cert = new X509Certificate2(data);
                    cert_collection.Add(rootCA_cert);
                    StoreCertificate(ref rootCAViewModel, ref rootCA_cert);
                    progress += 6;
                }
                else if (String.Equals(label, "CA", StringComparison.Ordinal))
                {
                    progress_info = "reading intermediate certificate";
                    intermediateCA_cert = new X509Certificate2(data);
                    cert_collection.Add(intermediateCA_cert);
                    StoreCertificate(ref intermediateCAViewModel, ref intermediateCA_cert);
                    progress += 6;
                }
                else if (String.Equals(label, "CERT_RN_FILE", StringComparison.Ordinal))
                {
                    progress_info = "reading RN certificate";
                    RN_cert = new X509Certificate2(data);
                    cert_collection.Add(RN_cert);
                    StoreCertificate(ref RNCertViewModel, ref RN_cert);
                    progress += 6;
                }
            }
            catch (Exception e)
            {
                this.logText += "An error occurred storing binary data of " + label + "\n";
                Console.WriteLine("An error occurred: '{0}'", e);
            }
        }

        public void ResetDataValues()
        {
            text_color = "Gray";
            firstName = "-";
            firstNames = "-";
            surName = "-";
            first_letter_of_third_given_name = "-";
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
            eid_card_present = false;
            progress = 0;
            HideProgressBar();
            pinop_ready = false;
            print_enabled = false;

            cert_collection = new X509Certificate2Collection();
        }

        public void NotifyPropertyChanged(String propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
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

        private string _type_kaart;
        public string type_kaart
        {
            get { return _type_kaart; }
            set
            {
                _type_kaart = value;
                NotifyPropertyChanged("type_kaart");
            }
        }

        private string _firstName;
        public string firstName
        {
            get { return _firstName; }
            set
            {
                _firstName = value;
                _firstNames = value + _first_letter_of_third_given_name;
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
                _firstNames = _firstName + value;
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

        private string _document_type;
        private string _document_type_value;
        public string document_type
        {
            get { return _document_type; }
            set
            {
                _document_type_value = value;
                if (String.Equals(value, "1", StringComparison.Ordinal))
                {
                    ResourceManager rm = new ResourceManager("IdentityTabStringResources",
                                         typeof(BackendDataViewModel).Assembly);
                    _document_type = rm.GetString("type_BELGIAN_CITIZEN", null);

                }
                this.NotifyPropertyChanged("document_type");
            }
        }

        public void Refresh()
        {
            document_type = _document_type_value;
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

        private ObservableCollection<ReadersMenuViewModel> _readersList;
        public ObservableCollection<ReadersMenuViewModel> readersList
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
