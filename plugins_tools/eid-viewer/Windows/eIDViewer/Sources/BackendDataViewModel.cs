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
using System.Collections.Concurrent;
using System.Reflection;
using System.Threading;
using System.Globalization;

namespace eIDViewer
{
    public partial class BackendDataViewModel : INotifyPropertyChanged
    {

        private readonly SynchronizationContext _syncContext;

        public BackendDataViewModel()
        {
            // we assume this ctor is called from the UI thread!
            _syncContext = SynchronizationContext.Current;

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
            eid_backend_state = eid_vwr_states.STATE_COUNT;//this is a invalid state, using it as initialization value

            //try to find a log_level setting in the registry, 
            //and initialize log_level and log_level_index
            GetViewerLogLevel();
        }

        ~BackendDataViewModel()
        {
            Dispose(false);
        }


        public void ShowPINVerifiedOKCallback(string message)
        {
            try
            {
                ResourceManager rm = new ResourceManager("eIDViewer.Resources.ApplicationStringResources",
                        Assembly.GetExecutingAssembly());
                CultureInfo culture = Thread.CurrentThread.CurrentCulture;
                System.Windows.MessageBox.Show(rm.GetString(message, culture));
            }
            catch (Exception e)
            {
                this.WriteLog("Exception in function ShowPINVerifiedOKCallback: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
        }

        public void pincodeVerifiedSucces(string message)
        {
            _syncContext.Post(o => ShowPINVerifiedOKCallback(message), null);
        }

        //hashAlg being "SHA1", "SHA256" or "SHA384"
        public bool CheckRNSignature(byte[] data, byte[] signedHash, string hashAlg)
        {
            byte[] HashValue;

            try
            {
                if (carddata_appl_version < 0x18)
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
                        WriteLog("The signature of the data is valid \n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);
                        return true;
                    }
                    else
                    {
                        //check if this is not a re-keyed card that received a hash upgrade sha1 -> sha256
                        if (hashAlg.Equals("SHA1"))
                        {
                            this.WriteLog("The SHA1 signature of the data is invalid, checking if the card is re-keyed \n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                            return CheckRNSignature(data, signedHash, "SHA256");
                        }
                        this.WriteLog("The signature of the data is not valid \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                        ResetDataValues();
                    }
                }
                else if (carddata_appl_version == 0x18)
                {
                    //ASN.1 structure:
                    //SEQ (0x30) / PayloadLen / Int Type (0x02) / Len (r) / r[] / Int Type (0x02) / Len (s) / s[]

                    //we only support keys with length not over 0x80
                    byte rLen = 0; //Length of r value as in ASN.1 signature
                    byte sLen = 0; //Length of s value as in ASN.1 signature

                    byte[] rawData; //the concatenation of 2 byte arrays, each the same size as the EC key

                    //first byte needs to be sequence
                    if (signedHash[0] == 0x30)
                    {
                        //int totalLen = signedHash[1];
                        if (signedHash[2] == 0x02)   //r value should be of int type
                        {
                            rLen = signedHash[3];

                            if (signedHash[4 + rLen] == 0x02)  //s value should be of int type
                            {
                                sLen = signedHash[5 + rLen];

                                rawData = new byte[96];
                                //array is 0 initialized (needed as the possible prepending zero's)

                                if (rLen <= 48)
                                {
                                    //add r value to rawdata (in the first 48 bytes, with prepended zero's added in the raw signature if needed )
                                    Array.Copy(signedHash, 4, rawData, 48 - rLen, rLen);
                                }
                                else
                                {
                                    //add r value to rawdata (in the first 48 bytes, drop the prepended zero's from the asn.1 signature if needed )
                                    Array.Copy(signedHash, 4 + rLen - 48, rawData, 0, 48);
                                }

                                if (sLen <= 48)
                                {
                                    //add s value to rawdata (in the next 48 bytes, with prepended zero's if needed )
                                    Array.Copy(signedHash, 6 + rLen, rawData, 96 - sLen, sLen);
                                }
                                else
                                {
                                    //add s value to rawdata (in the next 48 bytes, drop the prepended zero's from the asn.1 signature if needed )
                                    Array.Copy(signedHash, 6 + rLen + sLen - 48, rawData, 48, 48);
                                }


                                ECDsaCng thepublicKey = (ECDsaCng)RN_cert.GetECDsaPublicKey();
                                {
                                    thepublicKey.HashAlgorithm = CngAlgorithm.Sha384;
                                    if (thepublicKey.VerifyData(data, rawData, HashAlgorithmName.SHA384))
                                    {
                                        WriteLog("The EC signature of the data is valid \n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);
                                        return true;
                                    }
                                    else
                                    {
                                        this.WriteLog("The EC signature of the data is not valid \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                                    }
                                }
                            }
                            else
                            {
                                //s value not of type int
                                this.WriteLog("Format error in asn.1 signature (type of s not int (0x02)), unable to start validation\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                            }
                        }
                        else
                        {
                            //r value not of type int
                            this.WriteLog("Format error in asn.1 signature (type of r not int (0x02)), unable to start validation\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                        }
                    }
                    else
                    {
                        //error out, signedHash not starting with SEQ
                        this.WriteLog("Format error in asn.1 signature (doesn't start with SEQ (0x30)), unable to start validation\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                    }

                }
            }
            catch (Exception e)
            {
                this.WriteLog("An error occurred validating the data signature\n Exception message is: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
            ResetDataValues();
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
                else if (shaHash.Length == 48)
                {
                    SHA384 sha = new SHA384CryptoServiceProvider();
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
                this.WriteLog("An error occurred computing a sha1 hash \nException message: " + e.Message + "\n" , eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
            return false;

        }

        //verify if the chain that was build by .NET is identical (or with cross-signed root) to the one on the eID Card
        public bool CheckChain(ref X509Chain buildChain, ref X509Certificate2 leafCertificate)
        {
            int chainLen = 3;

            try
            {

                if (leafCertificate.Equals(RN_cert))
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
                    this.WriteLog("certificate chain not build correctly, leafCertificate in Windows store differs from the one on eID card \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                    return false;
                }
                if (chainLen == 2)
                {
                    if (!buildChain.ChainElements[1].Certificate.GetPublicKey().SequenceEqual(rootCA_cert.GetPublicKey()))
                    {
                        //root cert in the verified chain has different public key then the one on the eID Card
                        this.WriteLog("certificate chain not build correctly, RootCA in Windows store has different public key then the one on the eID Card \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                        return false;
                    }
                }
                //check if entire chain .NET just build is the one on the eID Card
                else
                {
                    if (buildChain.ChainElements[1].Certificate.Thumbprint != intermediateCA_cert.Thumbprint)
                    {
                        //intermediateCA cert  in the verified chain is not the one on the eID Card
                        this.WriteLog("certificate chain not build correctly, intermediateCA in Windows store differs from the one on eID card \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                        return false;
                    }
                    //check if root cert in the verified chain has different public key then the one on the eID Card
                    if (!buildChain.ChainElements[2].Certificate.GetPublicKey().SequenceEqual(rootCA_cert.GetPublicKey()))
                    {
                        //root cert in the verified chain has different public key then the one on the eID Card
                        this.WriteLog("certificate chain not build correctly, RootCA in Windows store has different public key then the one on the eID Card \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                        return false;
                    }
                }
            }
            catch (Exception e)
            {
                this.WriteLog("Exception in function CheckChain: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
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

                        if (rootCertOnCard.GetPublicKeyString().Equals(fileCert.GetPublicKeyString()))
                        {
                            foundEmbeddedRootCA = true;
                        }
                    }
                    else
                    {
                        this.WriteLog("embeddedRootCA: %s not found\n" + embeddedRootCA + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                    }
                }
            }
            catch (CryptographicException e)
            {
                this.WriteLog("An error occurred comparing the Belgium rootCA on the card with the ones in the EIDViewer\n" + e.ToString(), eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
            return foundEmbeddedRootCA;
        }


        public bool VerifyRootCA(ref X509Certificate2 rootCertOnCard, ref CertViewModel rootViewModel)
        {
            bool foundEmbeddedRootCA = false;
            string[] embeddedRootCAs = { "eIDViewer.Resources.Certs.belgiumrca2.pem",
                                         "eIDViewer.Resources.Certs.belgiumrca3.pem",
                                         "eIDViewer.Resources.Certs.belgiumrca4.pem",
                                         "eIDViewer.Resources.Certs.belgiumrca6.pem"};

            if( (rootCertOnCard == null) || (rootViewModel == null) )
            {
                this.WriteLog("No root certificate present to verify\n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
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
                this.WriteLog("Unknow RootCA \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
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
            this.WriteLog("Chain Information \n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
            this.WriteLog("Chain revocation flag: " + chain.ChainPolicy.RevocationFlag + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
            this.WriteLog("Chain revocation mode: " + chain.ChainPolicy.RevocationMode + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
            this.WriteLog("Chain verification flag: " + chain.ChainPolicy.VerificationFlags + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
            this.WriteLog("Chain verification time: " + chain.ChainPolicy.VerificationTime + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
            this.WriteLog("Chain status length: " + chain.ChainStatus.Length + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
            this.WriteLog("Chain application policy count: " + chain.ChainPolicy.ApplicationPolicy.Count + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
            this.WriteLog("Chain certificate policy count: " + chain.ChainPolicy.CertificatePolicy.Count + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);

            //Output chain element information.
            this.WriteLog("Chain Element Information \n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
            this.WriteLog("Number of chain elements: " + chain.ChainElements.Count + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
            this.WriteLog("Chain elements synchronized? " + chain.ChainElements.IsSynchronized + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
        }

        public void LogChainElement(ref X509ChainElement element, int index)
        {
            this.WriteLog("Certificate in chain \n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
            this.WriteLog("subject: " + element.Certificate.Subject + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
            this.WriteLog("issuer name: " + element.Certificate.Issuer + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
            this.WriteLog("valid from: " + element.Certificate.NotBefore + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
            this.WriteLog("valid until: " + element.Certificate.NotAfter + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
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
                    chain.ChainPolicy.RevocationMode = X509RevocationMode.Online;//.NoCheck for testing X509RevocationMode.Online;
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
                        this.WriteLog("no valid certificate chain could be constructed \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                        chainStatus = eid_cert_status.EID_CERT_STATUS_UNKNOWN;
                    }
                    else
                    {
                        //a valid certificate chain is constructed, now verify if it is the same as the one on the eID Card
                        if (CheckChain(ref chain, ref leafCertificate))
                        {
                            this.WriteLog("certificate chain build correctly \n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
                            chainStatus = eid_cert_status.EID_CERT_STATUS_VALID;
                        }
                        else
                        {
                            this.WriteLog("this was not the certificate chain we were looking for \n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
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
                                this.WriteLog("certificate status is " + element.ChainElementStatus[index].Status + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
                                this.WriteLog("certificate status information: " + element.ChainElementStatus[index].StatusInformation + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);

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
                    this.WriteLog("Leaf certificate was null, cannot verify \n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                }
            }

            catch (Exception e)
            {
                this.WriteLog("An error occurred checking the certificate status \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                this.WriteLog("Exception message: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
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
                this.WriteLog("this root certificate is not know by this version of the eID Viewer \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
//                this.WriteLog("Either you are not running the latest version of the eID Viewer, or there is an issue with this eID Card", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
            progress_info = "";
        }

        public void VerifyAllData()
        {
            if( (IsVerifiedDataOK() == false) || (IsBasicKeyOK() == false) )
            {
                ResetDataValues();
                eid_data_ready = false;
                eIDViewer.NativeMethods.MarkCardInvalid();
                return;
            }
            eid_data_ready = true;
            if (validateAlways == true)
            {
                VerifyAllCertificates();
            }
        }

        public bool IsBasicKeyOK()
        {
            //never verify the basic key on applet 1.7 cards
            if (carddata_appl_version > 0x17 )
            {
                if (basicKeyHash == null)
                {
                    //no basickeyhash field found
                    this.WriteLog("no public basic key hash is found, but it is mandatory starting from applet 1.8 cards \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                    return false;
                }

                if (basicKeyHash.Length != 48)
                {
                    //only hashAlg used is SHA384
                    this.WriteLog("public basic key hash is present, but it is not 48 bytes long, " + basicKeyHash.Length + " bytes were found\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                    return false;
                }
                if (basicKeyFile == null)
                {
                    //if we have a basic key hash in the signed ID FILE, the card needs to have a matching basic key file
                    this.WriteLog("public basic key hash is present, but no matching basic key file is present\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                    return false;
                }
                //check if the public basic key corresponds with the basic key hash in the identity file
                if (CheckShaHash(basicKeyFile, basicKeyHash) != true)
                {
                    this.WriteLog("public basic key doesn't match the hash in the ID file \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                    return false;
                }
                else
                {
                    /* temporary disable this check, only 5000 challenges can be requested without PIN
                     * probably we'll make this an optional check in the viewer
                     * 
                    //public basic key is correct, now check if the card contains the matching private key
                    challenge = new byte[48];

                    Random rnd = new Random();
                    rnd.NextBytes(challenge);

                    SHA384 sha = new SHA384CryptoServiceProvider();
                    byte[] challenge_hash = sha.ComputeHash(challenge, 0,48);
                    byte challenge_hash_len = 48;

                    if (eIDViewer.NativeMethods.DoChallenge(challenge_hash, challenge_hash_len) == 0)
                    {
                        this.WriteLog("validating the card authenticity: challenging the card basic key \n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);
                    }
                    else
                    {
                        this.WriteLog("validating the card authenticity: backend failed sending challenge to the card \n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                    }
                    */
                }
            }
            return true;
        }

        public bool IsVerifiedDataOK( )
        {
            string hashAlg;
            try {
                if (photo_hash.Length == 20)
                {
                    hashAlg = "SHA1";
                }
                else if (photo_hash.Length == 32)
                {
                    hashAlg = "SHA256";
                }
                else if (photo_hash.Length == 48)
                {
                    hashAlg = "SHA384";
                }
                else
                {
                    return false;
                }

                //check if the identity signature is ok
                if (CheckRNSignature(dataFile, dataSignFile, hashAlg) != true)
                {
                    this.WriteLog("identity dataFile signature check failed \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                    return false;
                }

                //check if the address signature is ok
                byte[] trimmedAddressFile = (byte[])addressFile.Clone();
                int lastIndex = Array.FindLastIndex(trimmedAddressFile, b => b != 0);
                Array.Resize(ref trimmedAddressFile, lastIndex + 1 + dataSignFile.Length);
                dataSignFile.CopyTo(trimmedAddressFile, lastIndex + 1);

                if (CheckRNSignature(trimmedAddressFile, addressSignFile, hashAlg) != true)
                {
                    this.WriteLog("addressFile signature check failed \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                    return false;
                }

                //check if the photo corresponds with the photo hash in the identity file
                if (CheckShaHash(photoFile, photo_hash) != true)
                {
                    this.WriteLog("photo doesn't match the hash in the signature file \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                    return false;
                }
            }
            catch (Exception e)
            {
                this.WriteLog("Error occuder during verification of data \nException message: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
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

        private void StoreCertificate(ref CertViewModel theCertViewModel, ref X509Certificate2 theX509Certificate)
        {
            try
            {

                theCertViewModel.Cert = theX509Certificate;
                theCertViewModel.CertValidfrom = theX509Certificate.GetEffectiveDateString();
                theCertViewModel.CertValidUntill = theX509Certificate.GetExpirationDateString();
                theCertViewModel.CertVisibility = Visibility.Visible;
                theCertViewModel.CertTrust = "Not checked";
                theCertViewModel.CertSubject = theX509Certificate.Subject.Replace(',', '\n');
                theCertViewModel.CertLabel = theX509Certificate.SubjectName.Name;
                string subjectName = theX509Certificate.SubjectName.Name;
                // This search returns the substring between two strings, so 
                // the first index is moved to the character just after the first string.
                Int32 first = subjectName.IndexOf("CN=") + "CN=".Length;
                if (first < 1)
                {
                    //no CN field found, just clear the certlabel
                    theCertViewModel.CertLabel = "";
                }
                else
                {
                    Int32 last = subjectName.IndexOf(",", first);
                    if (last == -1)
                    {
                        last = subjectName.Length - 1;
                    }
                    theCertViewModel.CertLabel = subjectName.Substring(first, last - first);
                }

                theCertViewModel.CertNotifyPropertyChanged("CertLabel");

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
            catch (Exception e)
            {
                this.WriteLog("Exception in function StoreCertificate: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
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
                this.WriteLog("An error occurred displaying the image \nException message: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                return null;
            }
        }

        private void ForeignersFieldPresent()
        {
            foreigner_fields_height = 26;
            foreigner_fields_height_double = 42;
        }

        private void BrexitFieldPresent()
        {
            brexit_fields_height = 26;
        }

        private void ShowCardAppletVersion(byte carddata_appl_version)
        {
            switch(carddata_appl_version)
            {
                case 0x17:
                    applet_version = "1.7";
                    break;
                case 0x18:
                    applet_version = "1.8";
                    break;
                default:
                    applet_version = "?";
                    break;
            }
        }

        public void StoreStringData(string label, string data)
        {

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
            {
                if (data != "")
                { nobility = data; }
            }
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
                ForeignersFieldPresent(); }
            else if (String.Equals(label, "special_organization", StringComparison.Ordinal))
            { special_organisation = data;
                ForeignersFieldPresent(); }         
            else if (String.Equals(label, "duplicata", StringComparison.Ordinal))
            { duplicate = data;
                ForeignersFieldPresent(); }
            else if (String.Equals(label, "work_permit_mention", StringComparison.Ordinal))
            { work_permit_mention = data;
                ForeignersFieldPresent(); }
            else if (String.Equals(label, "employers_vat_number1", StringComparison.Ordinal))
            { employers_vat_number1 = data;
                ForeignersFieldPresent(); }
            else if (String.Equals(label, "employers_vat_number2", StringComparison.Ordinal))
            { employers_vat_number2 = data;
                ForeignersFieldPresent(); }
            else if (String.Equals(label, "regional_file_number", StringComparison.Ordinal))
            { regional_file_number = data;
                ForeignersFieldPresent(); }
            else if (String.Equals(label, "brexit_mention_1", StringComparison.Ordinal))
            { brexit_mention1 = data;
                BrexitFieldPresent(); }
            else if (String.Equals(label, "brexit_mention_2", StringComparison.Ordinal))
            { brexit_mention2 = data;
                BrexitFieldPresent(); }
        }

        private byte[] dataFile;
        private byte[] addressFile;
        private byte[] dataSignFile;
        private byte[] addressSignFile;
        private byte[] photoFile;
        private byte[] photo_hash;
        private byte[] basicKeyHash;
        public byte[] basicKeyFile;

        public void StoreBinData(string label, byte[] data, int datalen)
        {
            try
            {            
                if (String.Equals(label, "PHOTO_FILE", StringComparison.Ordinal))
                {
                    photoFile = new byte[datalen];
                    data.CopyTo(photoFile, 0);
                    photo = LoadImage(data);
                    progress_info = "read photo";
                    progress += 4;
                }
                else if (String.Equals(label, "photo_hash", StringComparison.Ordinal))
                {
                    photo_hash = new byte[datalen];
                    data.CopyTo(photo_hash, 0);
                    progress_info = "read the identity data file";
                    progress += 4;
                }
                else if (String.Equals(label, "DATA_FILE", StringComparison.Ordinal))
                {
                   dataFile = new byte[datalen];
                   data.CopyTo(dataFile,0);
                   progress_info = "read the identity data file";
                   progress += 4;                         
                }
                else if (String.Equals(label, "ADDRESS_FILE", StringComparison.Ordinal))
                {
                    addressFile = new byte[datalen];
                    data.CopyTo(addressFile, 0);
                    progress_info = "read the address file";
                    progress += 2;
                }
                else if (String.Equals(label, "SIGN_DATA_FILE", StringComparison.Ordinal))
                {
                    dataSignFile = new byte[datalen];
                    data.CopyTo(dataSignFile, 0);
                    progress_info = "read the signature of the identity file";
                    progress += 1;
                }
                else if (String.Equals(label, "SIGN_ADDRESS_FILE", StringComparison.Ordinal))
                {
                    addressSignFile = new byte[datalen];
                    data.CopyTo(addressSignFile, 0);
                    progress_info = "read the signature of the address file";
                    progress += 1;
                }
                else if (String.Equals(label, "chip_number", StringComparison.Ordinal))
                { //chip_number = BitConverter.ToString(data);
                    progress_info = "read chip_number";
                    chip_number = String.Concat(Array.ConvertAll(data, x => x.ToString("X2")));
                    progress += 1;
                }
                else if (String.Equals(label, "Authentication", StringComparison.Ordinal))
                {
                    progress_info = "read authentication certificate";
                    authentication_cert = new X509Certificate2(data);
                    StoreCertificate( ref authCertViewModel, ref authentication_cert);
                    progress += 6;
                }
                else if (String.Equals(label, "Signature", StringComparison.Ordinal))
                {
                    progress_info = "read signature certificate";
                    signature_cert = new X509Certificate2(data);
                    StoreCertificate(ref signCertViewModel, ref signature_cert);
                    progress += 6;
                }
                else if (String.Equals(label, "Root", StringComparison.Ordinal))
                {
                    progress_info = "read root certificate";
                    rootCA_cert = new X509Certificate2(data);
                    StoreCertificate(ref rootCAViewModel, ref rootCA_cert);
                    progress += 6;
                }
                else if (String.Equals(label, "CA", StringComparison.Ordinal))
                {
                    progress_info = "read intermediate certificate";
                    intermediateCA_cert = new X509Certificate2(data);
                    StoreCertificate(ref intermediateCAViewModel, ref intermediateCA_cert);
                    progress += 6;
                }
                else if (String.Equals(label, "CERT_RN_FILE", StringComparison.Ordinal))
                {
                    progress_info = "read RN certificate";
                    RN_cert = new X509Certificate2(data);
                    StoreCertificate(ref RNCertViewModel, ref RN_cert);
                    progress += 6;
                }
                else if (String.Equals(label, "member_of_family", StringComparison.Ordinal))
                {
                    member_of_family = true;
                    ForeignersFieldPresent();
                }
                else if (String.Equals(label, "carddata_appl_version", StringComparison.Ordinal))
                {
                    carddata_appl_version = data[0];
                    ShowCardAppletVersion(carddata_appl_version);
                }
                else if (String.Equals(label, "basic_key_hash", StringComparison.Ordinal))
                {
                    basicKeyHash = new byte[datalen];
                    data.CopyTo(basicKeyHash, 0);
                    progress_info = "read the identity data file";
                    progress += 1;
                }
                else if (String.Equals(label, "BASIC_KEY_FILE", StringComparison.Ordinal))
                {
                    basicKeyFile = new byte[datalen];
                    data.CopyTo(basicKeyFile, 0);
                    progress_info = "read the identity data file";
                    progress += 1;
                }
            }
            catch (Exception e)
            {
                this.WriteLog("An error occurred storing binary data of " + label + "\nException message: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
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
            applet_version = "-";
            card_number = "-";
            issuing_municipality = "-";
            chip_number = "-";
            validity_begin_date = "-";
            validity_end_date = "-";
            date_and_country_of_protection = "-";
            member_of_family = false;
            special_organisation = "-";
            duplicate = "-";
            work_permit_mention = "-";
            employers_vat_number1 = "-";
            employers_vat_number2 = "-";
            regional_file_number = "-";
            brexit_mention1 = "-";
            brexit_mention2 = "-";
            eid_card_present = false;
            progress = 0;
            HideProgressBar();
            foreigner_fields_height = 0;
            foreigner_fields_height_double = 0;
            brexit_fields_height = 0;
            pinop_ready = false;
            open_enabled = true;

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

            dataFile = null;
            addressFile = null;
            dataSignFile = null;
            addressSignFile = null;
            photoFile = null;
            photo_hash = null;
            basicKeyHash = null;
            basicKeyFile = null;
        }

        public void WriteLog(String logLine, eid_vwr_loglevel loglevelofLine)
        {
            if (log_level <= loglevelofLine)
            {
                switch (loglevelofLine)
                {
                    case eid_vwr_loglevel.EID_VWR_LOG_ERROR:
                        logText += "E: ";
                        break;
                    case eid_vwr_loglevel.EID_VWR_LOG_COARSE:
                        logText += "W: ";
                        break;
                    case eid_vwr_loglevel.EID_VWR_LOG_NORMAL:
                        logText += "N: ";
                        break;
                    case eid_vwr_loglevel.EID_VWR_LOG_DETAIL:
                        logText += "D: ";
                        break;
                    default:
                        break;
                }
                logText += logLine;
            }
        }

        public eid_vwr_states eid_backend_state { get; set; }

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

        private Boolean _eid_read_data_started;
        public Boolean eid_read_data_started
        {
            get { return _eid_read_data_started; }
            set
            {
                _eid_read_data_started = value;
                this.NotifyPropertyChanged("eid_read_data_started");
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

        public byte carddata_appl_version = 0x00;

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

        private string _applet_version;
        public string applet_version
        {
            get { return _applet_version; }
            set
            {
                _applet_version = value;
                this.NotifyPropertyChanged("applet_version");
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

        private string _special_organisation;
        public string special_organisation
        {
            get { return _special_organisation; }
            set
            {
                _special_organisation = value;
                this.NotifyPropertyChanged("special_organisation");
            }
        }

        private string _duplicate;
        public string duplicate
        {
            get { return _duplicate; }
            set
            {
                _duplicate = value;
                this.NotifyPropertyChanged("duplicate");
            }
        }

        private string _work_permit_mention;
        public string work_permit_mention
        {
            get { return _work_permit_mention; }
            set
            {
                _work_permit_mention = value;
                this.NotifyPropertyChanged("work_permit_mention");
            }
        }

        private string _employers_vat_number1;
        public string employers_vat_number1
        {
            get { return _employers_vat_number1; }
            set
            {
                _employers_vat_number1 = value;
                this.NotifyPropertyChanged("Employers_vat_number1");
            }
        }

        private string _employers_vat_number2;
        public string employers_vat_number2
        {
            get { return _employers_vat_number2; }
            set
            {
                _employers_vat_number2 = value;
                this.NotifyPropertyChanged("Employers_vat_number2");
            }
        }

        private string _regional_file_number;
        public string regional_file_number
        {
            get { return _regional_file_number; }
            set
            {
                _regional_file_number = value;
                this.NotifyPropertyChanged("regional_file_number");
            }
        }

        private string _brexit_mention1;
        public string brexit_mention1
        {
            get { return _brexit_mention1; }
            set
            {
                _brexit_mention1 = value;
                this.NotifyPropertyChanged("brexit_mention1");
            }
        }

        private string _brexit_mention2;
        public string brexit_mention2
        {
            get { return _brexit_mention2; }
            set
            {
                _brexit_mention2 = value;
                this.NotifyPropertyChanged("brexit_mention2");
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

        private int _foreigner_fields_height_double;
        public int foreigner_fields_height_double
        {
            get { return _foreigner_fields_height_double; }
            set
            {
                _foreigner_fields_height_double = value;
                this.NotifyPropertyChanged("foreigner_fields_height_double");
            }
        }

        private int _brexit_fields_height;
        public int brexit_fields_height
        {
            get { return _brexit_fields_height; }
            set
            {
                _brexit_fields_height = value;
                this.NotifyPropertyChanged("brexit_fields_height");
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

        private eid_vwr_loglevel _log_level;
        public eid_vwr_loglevel log_level
        {
            get { return _log_level; }
            set
            {
                _log_level = value;
                this.NotifyPropertyChanged("log_level");
            }
        }

        private int _log_level_index;
        public int log_level_index
        {
            get { return _log_level_index; }
            set
            {
                _log_level_index = value;
                this.NotifyPropertyChanged("log_level_index");
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

        private Boolean _open_enabled = false;
        public Boolean open_enabled
        {
            get { return _open_enabled; }
            set
            {
                _open_enabled = value;
                this.NotifyPropertyChanged("open_enabled");
            }
        }

        private byte[] _challenge = null;
        public byte[] challenge
        {
            get { return _challenge; }
            set
            {
                _challenge = value;
                this.NotifyPropertyChanged("challenge");
            }
        }

    }
}
