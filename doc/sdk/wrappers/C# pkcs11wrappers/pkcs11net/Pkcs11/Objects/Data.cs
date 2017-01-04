using System;
using System.Collections.Generic;
using System.Text;

using Net.Sf.Pkcs11.Wrapper;

namespace Net.Sf.Pkcs11.Objects
{
    public class Data : Storage
    {
        protected CharArrayAttribute application = new CharArrayAttribute(CKA.APPLICATION);
        protected ByteArrayAttribute objectID = new ByteArrayAttribute(CKA.OBJECT_ID);
        protected ByteArrayAttribute value_ = new ByteArrayAttribute(CKA.VALUE);

        public CharArrayAttribute Application
        {
            get { return application; }
        }

        public ByteArrayAttribute ObjectID
        {
            get { return objectID; }
        }

        public ByteArrayAttribute Value
        {
            get { return value_; }
        }


        public static new P11Object GetInstance(Session session, uint hObj)
        {
            return new Data(session, hObj);
        }

        public Data(Session session, uint hObj)
            : base(session, hObj)
        {
        }

        public Data()
        {
            Class.ObjectType = CKO.DATA;
        }

        public override void ReadAttributes(Session session)
        {
            base.ReadAttributes(session);

            application = ReadAttribute(session, HObj, new CharArrayAttribute(CKA.APPLICATION));
            objectID = ReadAttribute(session, HObj, new ByteArrayAttribute(CKA.OBJECT_ID));
            value_ = ReadAttribute(session, HObj, new ByteArrayAttribute(CKA.VALUE));
        }

        public override string ToString()
        {
            if (application.Value != null)
                return "Data object: " + new string(application.Value);
            else if (objectID.Value != null)
                return "Data object: " + System.Text.Encoding.ASCII.GetString(objectID.Value);
            else
                return "Data object: " + base.ToString();
        }

    }
}
