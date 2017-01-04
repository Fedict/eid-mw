
using System;
using Net.Sf.Pkcs11.Wrapper;
namespace Net.Sf.Pkcs11.Objects
{
    /// <summary>
    /// Description of Storage.
    /// </summary>
    public class Storage : P11Object
    {
        protected BooleanAttribute token_ = new BooleanAttribute(CKA.TOKEN);
        protected BooleanAttribute private_ = new BooleanAttribute(CKA.PRIVATE);
        protected BooleanAttribute modifiable_ = new BooleanAttribute(CKA.MODIFIABLE);
        protected CharArrayAttribute label_ = new CharArrayAttribute(CKA.LABEL);

        public BooleanAttribute Token
        {
            get { return token_; }
        }

        public BooleanAttribute Private
        {
            get { return private_; }
        }

        public BooleanAttribute Modifiable
        {
            get { return modifiable_; }
        }

        public CharArrayAttribute Label
        {
            get { return label_; }
        }


        public Storage(Session session, uint hObj)
            : base(session, hObj)
        {
        }

        public Storage()
            : base()
        {

        }

        public override void ReadAttributes(Session session)
        {
            base.ReadAttributes(session);

            token_ = ReadAttribute(session, HObj, new BooleanAttribute(CKA.TOKEN));

            private_ = ReadAttribute(session, HObj, new BooleanAttribute(CKA.PRIVATE));

            modifiable_ = ReadAttribute(session, HObj, new BooleanAttribute(CKA.MODIFIABLE));

            label_ = ReadAttribute(session, HObj, new CharArrayAttribute(CKA.LABEL));
        }

        public override string ToString()
        {
            if (label_.Value != null)
                return new string(label_.Value);
            else
                return base.ToString();
        }
    }
}
