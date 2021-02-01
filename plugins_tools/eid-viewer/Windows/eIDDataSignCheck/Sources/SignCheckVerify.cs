using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;

namespace eIDDataSignCheck
{
    public partial class eIDDataSignCheckViewModel : INotifyPropertyChanged
    {
        public void VerifyAll()
        {
            if (IsVerifiedDataOK())
            {
                allSignaturesValid = true;
            }
            else
            {
                allSignaturesValid = false;
            }
        }

        public bool IsVerifiedDataOK()
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
                //this.WriteLog("identity dataFile signature check failed \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                return false;
            }

            //check if the address signature is ok
            byte[] trimmedAddressFile = (byte[])addressFile.Clone();
            int lastIndex = Array.FindLastIndex(trimmedAddressFile, b => b != 0);
            Array.Resize(ref trimmedAddressFile, lastIndex + 1 + dataSignFile.Length);
            dataSignFile.CopyTo(trimmedAddressFile, lastIndex + 1);

            if (CheckRNSignature(trimmedAddressFile, addressSignFile, hashAlg) != true)
            {
                //this.WriteLog("addressFile signature check failed \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                return false;
            }

            //check if the photo corresponds with the photo hash in the identity file
            if (CheckShaHash(photoFile, photo_hash) != true)
            {
                //this.WriteLog("photo doesn't match the hash in the signature file \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                return false;
            }

            return true;
        }

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

                X509Certificate2 RN_cert = new X509Certificate2(rnCertFile);
                RSACryptoServiceProvider csp = RN_cert.PublicKey.Key as RSACryptoServiceProvider;

                if (csp.VerifyHash(HashValue, CryptoConfig.MapNameToOID(hashAlg), signedHash))
                {
                    //WriteLog("The signature of the data is valid \n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);
                    return true;
                }
                else
                {
                    //check if this is not a re-keyed card that received a hash upgrade sha1 -> sha256
                    if (hashAlg.Equals("SHA1"))
                    {
                       // this.WriteLog("The SHA1 signature of the data is invalid, checking if the card is re-keyed \n", eid_vwr_loglevel.EID_VWR_LOG_COARSE);
                        return CheckRNSignature(data, signedHash, "SHA256");
                    }
                    //this.WriteLog("The signature of the data is not valid \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                    //ResetDataValues();
                }
            }
            catch (Exception e)
            {
              //  this.WriteLog("An error occurred validating the data signature\n Exception message is: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
            return false;
        }

        public bool CheckShaHash(byte[] data, byte[] shaHash)
        {
            byte[] HashValue;
            try
            {
                if (shaHash.Length == 20)
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
            //    this.WriteLog("An error occurred computing a sha1 hash \nException message: " + e.Message + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
            return false;

        }

    }
}
