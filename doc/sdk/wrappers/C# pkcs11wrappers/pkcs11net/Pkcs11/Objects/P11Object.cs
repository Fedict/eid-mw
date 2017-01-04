
using System;
using Net.Sf.Pkcs11.Wrapper;
using Net.Sf.Pkcs11;

namespace Net.Sf.Pkcs11.Objects
{
    /// <summary>
    /// Description of PKCS11Object.
    /// </summary>
    public class P11Object
    {
        private uint hObj;

        /// <summary>
        /// Handle of and object (read-only).
        /// </summary>
        public uint HObj
        {
            get { return hObj; }
            internal set { hObj = value; }
        }

        protected ObjectClassAttribute class_ = new ObjectClassAttribute();

        public ObjectClassAttribute Class
        {
            get { return class_; }
        }

        internal P11Object(Session session, uint hObj)
        {
            this.hObj = hObj;

            ReadAttributes(session);

        }
        internal P11Object()
        {
            this.hObj = 0;
        }

        public static P11Object GetInstance(Session session, uint hObj)
        {
            if (session == null)
                throw new Exception("Argument \"session\" must not be null.");

            ObjectClassAttribute classAtr = (ObjectClassAttribute)
                AssignAttributeFromObj(session, hObj, new ObjectClassAttribute());

            switch (classAtr.ObjectType)
            {
                case CKO.CERTIFICATE:
                    return Certificate.GetInstance(session, hObj);

                case CKO.DATA:
                    return Data.GetInstance(session, hObj);

                case CKO.DOMAIN_PARAMETERS:
                    return DomainParameters.GetInstance(session, hObj);

                case CKO.HW_FEATURE:
                    break;

                case CKO.MECHANISM:
                    break;

                case CKO.PRIVATE_KEY:
                    return PrivateKey.GetInstance(session, hObj);

                case CKO.PUBLIC_KEY:
                    return PublicKey.GetInstance(session, hObj);

                case CKO.SECRET_KEY:
                    return SecretKey.GetInstance(session, hObj);

                case CKO.VENDOR_DEFINED:
                    break;

                default:
                    break;
            }

            return null;
        }


        protected static P11Attribute AssignAttributeFromObj(Session session, uint hObj, P11Attribute attr)
        {
            uint hSession = session.HSession;
            Wrapper.Pkcs11Module pm = session.Module.P11Module;
            try
            {
                return attr.Load(pm.GetAttributeValue(hSession, hObj, new CK_ATTRIBUTE[] { attr.CK_ATTRIBUTE })[0]);
            }
            catch
            {//TODO:sadece attribute not found handle et
                return null;
            }
        }

        public static BooleanAttribute ReadAttribute(Session session, uint hObj, BooleanAttribute attr)
        {
            return (BooleanAttribute)GetAttribute(session, hObj, attr);
        }



        public static ByteArrayAttribute ReadAttribute(Session session, uint hObj, ByteArrayAttribute attr)
        {
            return (ByteArrayAttribute)GetAttribute(session, hObj, attr);
        }



        public static CertificateTypeAttribute ReadAttribute(Session session, uint hObj, CertificateTypeAttribute attr)
        {
            return (CertificateTypeAttribute)GetAttribute(session, hObj, attr);
        }



        public static CharArrayAttribute ReadAttribute(Session session, uint hObj, CharArrayAttribute attr)
        {
            return (CharArrayAttribute)GetAttribute(session, hObj, attr);
        }



        public static DateAttribute ReadAttribute(Session session, uint hObj, DateAttribute attr)
        {
            return (DateAttribute)GetAttribute(session, hObj, attr);
        }



        public static KeyTypeAttribute ReadAttribute(Session session, uint hObj, KeyTypeAttribute attr)
        {
            return (KeyTypeAttribute)GetAttribute(session, hObj, attr);
        }



        public static MechanismTypeAttribute ReadAttribute(Session session, uint hObj, MechanismTypeAttribute attr)
        {
            return (MechanismTypeAttribute)GetAttribute(session, hObj, attr);
        }



        public static ObjectClassAttribute ReadAttribute(Session session, uint hObj, ObjectClassAttribute attr)
        {
            return (ObjectClassAttribute)GetAttribute(session, hObj, attr);
        }



        public static UIntAttribute ReadAttribute(Session session, uint hObj, UIntAttribute attr)
        {
            return (UIntAttribute)GetAttribute(session, hObj, attr);
        }


        public virtual void ReadAttributes(Session session)
        {
            if (session == null)
                throw new NullReferenceException("Argument \"session\" must not be null.");
            class_ = ReadAttribute(session, hObj, new ObjectClassAttribute());
        }

        public static P11Attribute GetAttribute(Session session, uint hObj, P11Attribute attr)
        {
            try
            {
                uint hSession = session.HSession;
                Wrapper.Pkcs11Module pm = session.Module.P11Module;

                CK_ATTRIBUTE tmp = pm.GetAttributeValue(hSession, hObj, new CK_ATTRIBUTE[] { attr.CK_ATTRIBUTE })[0];
                P11Attribute p11 = attr.Clone().Load(tmp);
                p11.MetaData.IsPresent = true;
                p11.MetaData.IsSensitive = false;
                return p11;

            }
            catch (TokenException tex)
            {
                if (tex.ErrorCode == CKR.ATTRIBUTE_TYPE_INVALID
                   || tex.ErrorCode == CKR.ATTRIBUTE_VALUE_INVALID)
                {
                    P11Attribute p11 = attr.Clone();
                    p11.MetaData.IsPresent = false;
                    p11.MetaData.IsSensitive = false;
                    return p11;
                }
                else if (tex.ErrorCode == CKR.ATTRIBUTE_SENSITIVE)
                {
                    P11Attribute p11 = attr.Clone();
                    p11.MetaData.IsPresent = true;
                    p11.MetaData.IsSensitive = true;
                    return p11;
                }
                else
                    throw tex;

            }
        }
    }
}
