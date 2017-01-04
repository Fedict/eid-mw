
using System;
using Net.Sf.Pkcs11.Wrapper;
namespace Net.Sf.Pkcs11.Objects
{
    /// <summary>
    /// Description of GostPrivateKey.
    /// </summary>
    public class GostPrivateKey : PrivateKey
    {
        /// <summary>
        /// Params.
        /// </summary>
        protected ByteArrayAttribute params_ = new ByteArrayAttribute((uint)CKA.GOSTR3410PARAMS);
        public ByteArrayAttribute Params
        {
            get { return params_; }
        }

        public GostPrivateKey()
        {
            this.KeyType.KeyType = CKK.GOST;
            params_.Value = PKCS11Constants.SC_PARAMSET_GOSTR3410_A;
        }

        public GostPrivateKey(Session session, uint hObj)
            : base(session, hObj)
        {
            params_.Value = PKCS11Constants.SC_PARAMSET_GOSTR3410_A;
        }

        public static new P11Object GetInstance(Session session, uint hObj)
        {
            return new GostPrivateKey(session, hObj);
        }

        public override void ReadAttributes(Session session)
        {
            base.ReadAttributes(session);

            params_ = ReadAttribute(session, HObj, params_);
        }

    }
}
