
using System;
using Net.Sf.Pkcs11.Wrapper;
namespace Net.Sf.Pkcs11.Objects
{
    /// <summary>
    /// Description of PublicKey.
    /// </summary>
    public abstract class PublicKey : Key
    {
        public PublicKey()
        {
            Class.ObjectType = CKO.PUBLIC_KEY;
        }
        public PublicKey(Session session, uint hObj)
            : base(session, hObj)
        {
        }

        public static new P11Object GetInstance(Session session, uint hObj)
        {
            KeyTypeAttribute keyType = ReadAttribute(session, hObj, new KeyTypeAttribute());

            switch (keyType.KeyType)
            {
                case CKK.RSA:
                    return RSAPublicKey.GetInstance(session, hObj);
                case CKK.GOST:
                    return GostPublicKey.GetInstance(session, hObj);
                default:
                    return null;
            }

        }

        ByteArrayAttribute subject = new ByteArrayAttribute(CKA.SUBJECT);
        BooleanAttribute encrypt = new BooleanAttribute(CKA.ENCRYPT),
        verify = new BooleanAttribute(CKA.VERIFY),
        verifyRecover = new BooleanAttribute(CKA.VERIFY_RECOVER),
        wrap = new BooleanAttribute(CKA.WRAP),
        trusted = new BooleanAttribute(CKA.TRUSTED);

        public ByteArrayAttribute Subject
        {
            get { return subject; }
        }

        public BooleanAttribute Encrypt
        {
            get { return encrypt; }
        }

        public BooleanAttribute Verify
        {
            get { return verify; }
        }

        public BooleanAttribute VerifyRecover
        {
            get { return verifyRecover; }
        }

        public BooleanAttribute Wrap
        {
            get { return wrap; }
        }

        public BooleanAttribute Trusted
        {
            get { return trusted; }
        }


        public override void ReadAttributes(Session session)
        {
            base.ReadAttributes(session);

            subject = ReadAttribute(session, HObj, new ByteArrayAttribute(CKA.SUBJECT));
            encrypt = ReadAttribute(session, HObj, new BooleanAttribute(CKA.ENCRYPT));
            verify = ReadAttribute(session, HObj, new BooleanAttribute(CKA.VERIFY));
            verifyRecover = ReadAttribute(session, HObj, new BooleanAttribute(CKA.VERIFY_RECOVER));
            wrap = ReadAttribute(session, HObj, new BooleanAttribute(CKA.WRAP));
            trusted = ReadAttribute(session, HObj, new BooleanAttribute(CKA.TRUSTED));

        }

    }
}
