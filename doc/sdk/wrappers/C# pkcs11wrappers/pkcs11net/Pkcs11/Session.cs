
using System;
using System.Collections.Generic;
using System.Reflection;

using Net.Sf.Pkcs11.Objects;
using Net.Sf.Pkcs11.Wrapper;

namespace Net.Sf.Pkcs11
{
    /// <summary>
    /// Represents an open Session with a Token.
    /// </summary>
    public class Session : IDisposable
    {
        #region Members
        Token token;

        uint hSession; 
        #endregion

        #region Properties
       
        /// <summary>
        /// Session's Token
        /// </summary>
        public Token Token
        {
            get { return token; }
        }

        /// <summary>
        /// Session's Cryptoki Module
        /// </summary>
        public Module Module
        {
            get { return token.Module; }
        }

        /// <summary>
        /// Session Handle / id
        /// </summary>
        public uint HSession
        {
            get { return hSession; }
        } 

        #endregion

        #region Methods

        #region Instance

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="token">Session's Token</param>
        /// <param name="hSession">Session Handle / Id</param>
        public Session(Token token, uint hSession)
        {
            this.token = token;
            this.hSession = hSession;
        }

        #endregion
       
        #region Authentication

        public void Login(UserType userType, string pwd)
        {
            this.Module.P11Module.Login(this.HSession, (CKU)userType, pwd);
        }

        public void Logout()
        {
            this.Module.P11Module.Logout(hSession);
        }
        
        #endregion

        #region Initialization

        public void SetPIN(string oldPIN, string newPIN)
        {
            Module.P11Module.SetPIN(hSession, oldPIN, newPIN);
        }

        public void InitPIN(string pin)
        {
            Module.P11Module.InitPIN(hSession, pin);
        }

        #endregion

        #region Encipher

        #region Digest

        public void DigestInit(Mechanism mechanism)
        {
            this.Module.P11Module.DigestInit(hSession, mechanism.CK_MECHANISM);
        }

        public void DigestUpdate(byte[] data)
        {
            this.Module.P11Module.DigestUpdate(hSession, data);
        }

        public byte[] Digest(byte[] data)
        {
            return this.Module.P11Module.Digest(hSession, data);
        }

        public byte[] DigestFinal()
        {
            return this.Module.P11Module.DigestFinal(hSession);
        } 
        #endregion

        #region Encrypt

        public void EncryptInit(Mechanism mechanism, PublicKey key)
        {
            this.Module.P11Module.EncryptInit(hSession, mechanism.CK_MECHANISM, key.HObj);
        }

        public void EncryptInit(Mechanism mechanism, SecretKey key)
        {
            this.Module.P11Module.EncryptInit(hSession, mechanism.CK_MECHANISM, key.HObj);
        }

        public byte[] Encrypt(byte[] data)
        {
            return this.Module.P11Module.Encrypt(hSession, data);
        }

        public byte[] EncryptUpdate(byte[] data)
        {
            return this.Module.P11Module.EncryptUpdate(hSession, data);
        }

        public byte[] EncryptFinal()
        {
            return this.Module.P11Module.EncryptFinal(hSession);
        } 
        #endregion

        #region Decrypt

        public void DecryptInit(Mechanism mechanism, PrivateKey key)
        {
            this.Module.P11Module.DecryptInit(hSession, mechanism.CK_MECHANISM, key.HObj);
        }

        public void DecryptInit(Mechanism mechanism, SecretKey key)
        {
            this.Module.P11Module.DecryptInit(hSession, mechanism.CK_MECHANISM, key.HObj);
        }

        public byte[] Decrypt(byte[] data)
        {
            return this.Module.P11Module.Decrypt(hSession, data);
        }

        public byte[] DecryptUpdate(byte[] data)
        {
            return this.Module.P11Module.DecryptUpdate(hSession, data);
        }

        public byte[] DecryptFinal()
        {
            return this.Module.P11Module.DecryptFinal(hSession);
        } 
        #endregion

        #region Signature

        public void SignInit(Mechanism signingMechanism, PrivateKey key)
        {
            this.Module.P11Module.SignInit(hSession, signingMechanism.CK_MECHANISM, key.HObj);
        }

        public void SignUpdate(byte[] data)
        {
            this.Module.P11Module.SignUpdate(hSession, data);
        }

        public byte[] SignFinal()
        {
            return this.Module.P11Module.SignFinal(hSession);
        }

        public byte[] Sign(byte[] data)
        {
            return this.Module.P11Module.Sign(hSession, data);
        } 
        #endregion

        #region Verification

        public void VerifyInit(Mechanism signingMechanism, PublicKey key)
        {
            this.Module.P11Module.VerifyInit(hSession, signingMechanism.CK_MECHANISM, key.HObj);
        }

        public void VerifyInit(Mechanism signingMechanism, Certificate certificate)
        {
            this.Module.P11Module.VerifyInit(hSession, signingMechanism.CK_MECHANISM, certificate.HObj);
        }

        public void VerifyUpdate(byte[] data)
        {
            this.Module.P11Module.VerifyUpdate(hSession, data);
        }

        public bool VerifyFinal(byte[] signature)
        {
            try
            {
                this.Module.P11Module.VerifyFinal(hSession, signature);
                return true;
            }
            catch (TokenException tex)
            {
                if (tex.ErrorCode == CKR.SIGNATURE_INVALID) return false;
                throw tex;
            }
        }

        public bool Verify(byte[] data, byte[] signature)
        {

            try
            {
                this.Module.P11Module.Verify(hSession, data, signature);
                return true;
            }
            catch (TokenException tex)
            {
                if (tex.ErrorCode == CKR.SIGNATURE_INVALID) return false;
                throw tex;
            }
        }
        #endregion

        #region Key Generation

        public SecretKey GenerateKey(Mechanism mech, P11Object template)
        {
            uint hKey = this.Module.P11Module.GenerateKey(hSession, mech.CK_MECHANISM, getAssignedAttributes(template));
            return (SecretKey)SecretKey.GetInstance(this, hKey);
        }

        public KeyPair GenerateKeyPair(Mechanism mech, P11Object pubTemplate, P11Object privTemplate)
        {

            KeyPairHandler hkp = this.Module.P11Module.GenerateKeyPair(
                hSession,
                mech.CK_MECHANISM,
                getAssignedAttributes(pubTemplate),
                getAssignedAttributes(privTemplate)
            );

            return new KeyPair(
                (PublicKey)PublicKey.GetInstance(this, hkp.hPublicKey),
                (PrivateKey)PrivateKey.GetInstance(this, hkp.hPrivateKey)
            );
        }


        #endregion

        #endregion

        #region Objects
    
        #region Search

        public void FindObjectsInit(params P11Attribute[] attrs)
        {
            CK_ATTRIBUTE[] ckAttrs = P11Util.ConvertToCK_ATTRIBUTEs(attrs);
            this.Module.P11Module.FindObjectsInit(this.hSession, ckAttrs);
        }

        public P11Object[] FindObjects(uint maxCount)
        {
            uint[] hObjs = this.Module.P11Module.FindObjects(HSession, maxCount);
            P11Object[] objs = new P11Object[hObjs.Length];
            for (int i = 0; i < hObjs.Length; ++i)
            {
                objs[i] = P11Object.GetInstance(this, hObjs[i]);
            }
            return objs;
        }

        public void FindObjectsFinal()
        {
            this.Module.P11Module.FindObjectsFinal(hSession);
        }

        #endregion

        #region Management

        public P11Object CreateObject(P11Object template)
        {

            uint hObj = this.Module.P11Module.CreateObject(hSession, getAssignedAttributes(template));
            return P11Object.GetInstance(this, hObj);
        }

        public void DestroyObject(P11Object obj)
        {
            this.Module.P11Module.DestroyObject(hSession, obj.HObj);
        }

        #endregion 

        #endregion

        #region General

        public void Dispose()
        {
            CloseSession();
        }

        private void CloseSession()
        {
            this.Module.P11Module.CloseSession(hSession);
        }

        private static CK_ATTRIBUTE[] getAssignedAttributes(P11Object obj)
        {
            PropertyInfo[] props = obj.GetType().GetProperties();
            List<CK_ATTRIBUTE> attrs = new List<CK_ATTRIBUTE>();
            for (int i = 0; i < props.Length; i++)
            {
                P11Attribute val = props[i].GetValue(obj, null) as P11Attribute;
                if (val != null && val.IsAssigned)
                    attrs.Add(val.CK_ATTRIBUTE);
            }
            return attrs.ToArray();
        }

        #endregion

        #endregion
    }
}
