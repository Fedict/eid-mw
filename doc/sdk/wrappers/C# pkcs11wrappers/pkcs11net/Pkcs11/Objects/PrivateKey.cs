
using System;
using Net.Sf.Pkcs11.Wrapper;
namespace Net.Sf.Pkcs11.Objects
{
    /// <summary>
    /// Description of PrivateKey.
    /// </summary>
    public class PrivateKey : Key
    {

        ByteArrayAttribute subject = new ByteArrayAttribute(CKA.SUBJECT);

        public ByteArrayAttribute Subject
        {
            get { return subject; }
        }

        BooleanAttribute sensitive = new BooleanAttribute(CKA.SENSITIVE),
        decrypt = new BooleanAttribute(CKA.DECRYPT),
        sign = new BooleanAttribute(CKA.SIGN),
        signRecover = new BooleanAttribute(CKA.SIGN_RECOVER),
        unWrap = new BooleanAttribute(CKA.UNWRAP),
        extractable = new BooleanAttribute(CKA.SENSITIVE),
        alwaysSensitive = new BooleanAttribute(CKA.ALWAYS_SENSITIVE),
        neverExtractable = new BooleanAttribute(CKA.NEVER_EXTRACTABLE),
        wrapWithTrusted = new BooleanAttribute(CKA.WRAP_WITH_TRUSTED),
        alwaysAuthenticate = new BooleanAttribute(CKA.ALWAYS_AUTHENTICATE);

        public BooleanAttribute AlwaysAuthenticate
        {
            get { return alwaysAuthenticate; }
        }

        public BooleanAttribute WrapWithTrusted
        {
            get { return wrapWithTrusted; }
        }

        public BooleanAttribute NeverExtractable
        {
            get { return neverExtractable; }
        }

        public BooleanAttribute AlwaysSensitive
        {
            get { return alwaysSensitive; }
        }

        public BooleanAttribute Extractable
        {
            get { return extractable; }
        }

        public BooleanAttribute UnWrap
        {
            get { return unWrap; }
        }

        public BooleanAttribute SignRecover
        {
            get { return signRecover; }
        }

        public BooleanAttribute Sign
        {
            get { return sign; }
        }

        public BooleanAttribute Decrypt
        {
            get { return decrypt; }
        }

        public BooleanAttribute Sensitive
        {
            get { return sensitive; }
        }

        //TODO: CKA_UNWRAP_TEMPLATE 



        public PrivateKey()
        {
        }

        public PrivateKey(Session session, uint hObj)
            : base(session, hObj)
        {

        }


        public override void ReadAttributes(Session session)
        {

            base.ReadAttributes(session);

            subject = ReadAttribute(session, HObj, new ByteArrayAttribute(CKA.SUBJECT));
            sensitive = ReadAttribute(session, HObj, new BooleanAttribute(CKA.SENSITIVE));
            decrypt = ReadAttribute(session, HObj, new BooleanAttribute(CKA.DECRYPT));
            sign = ReadAttribute(session, HObj, new BooleanAttribute(CKA.SIGN));
            signRecover = ReadAttribute(session, HObj, new BooleanAttribute(CKA.SIGN_RECOVER));
            unWrap = ReadAttribute(session, HObj, new BooleanAttribute(CKA.UNWRAP));
            extractable = ReadAttribute(session, HObj, new BooleanAttribute(CKA.EXTRACTABLE));
            alwaysSensitive = ReadAttribute(session, HObj, new BooleanAttribute(CKA.ALWAYS_SENSITIVE));
            neverExtractable = ReadAttribute(session, HObj, new BooleanAttribute(CKA.NEVER_EXTRACTABLE));
            wrapWithTrusted = ReadAttribute(session, HObj, new BooleanAttribute(CKA.WRAP_WITH_TRUSTED));
            alwaysAuthenticate = ReadAttribute(session, HObj, new BooleanAttribute(CKA.ALWAYS_AUTHENTICATE));
        }


        public static new P11Object GetInstance(Session session, uint hObj)
        {

            KeyTypeAttribute keyAttr = ReadAttribute(session, hObj, new KeyTypeAttribute());


            switch (keyAttr.KeyType)
            {
                case CKK.RSA:
                    return RSAPrivateKey.GetInstance(session, hObj);
                case CKK.GOST:
                    return GostPrivateKey.GetInstance(session, hObj);
                default:
                    return null;
            }


        }
    }
}
