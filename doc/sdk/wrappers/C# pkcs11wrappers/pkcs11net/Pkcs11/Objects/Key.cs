
using System;
using Net.Sf.Pkcs11.Wrapper;
namespace Net.Sf.Pkcs11.Objects
{
    /// <summary>
    /// Description of Key.
    /// </summary>
    public abstract class Key : Storage
    {
        KeyTypeAttribute keyType = new KeyTypeAttribute();

        public KeyTypeAttribute KeyType
        {
            get { return keyType; }
        }

        ByteArrayAttribute id = new ByteArrayAttribute(CKA.ID);

        public ByteArrayAttribute Id
        {
            get { return id; }
        }

        DateAttribute startDate = new DateAttribute((uint)CKA.START_DATE);

        public DateAttribute StartDate
        {
            get { return startDate; }
        }

        DateAttribute endDate = new DateAttribute((uint)CKA.END_DATE);

        public DateAttribute EndDate
        {
            get { return endDate; }
        }

        BooleanAttribute derive = new BooleanAttribute(CKA.DERIVE);

        public BooleanAttribute Derive
        {
            get { return derive; }
        }

        BooleanAttribute local = new BooleanAttribute(CKA.LOCAL);

        public BooleanAttribute Local
        {
            get { return local; }
        }

        MechanismTypeAttribute keyGenMechanism = new MechanismTypeAttribute(CKA.KEY_GEN_MECHANISM);

        public MechanismTypeAttribute KeyGenMechanism
        {
            get { return keyGenMechanism; }
        }


        //TODO: CKA_ALLOWED_MECHANISMS 


        public Key()
        {
        }

        public Key(Session session, uint hObj)
            : base(session, hObj)
        {

        }

        public static new P11Object GetInstance(Session session, uint hObj)
        {
            return null;
        }

        public override void ReadAttributes(Session session)
        {
            base.ReadAttributes(session);

            keyType = ReadAttribute(session, HObj, new KeyTypeAttribute());

            id = ReadAttribute(session, HObj, new ByteArrayAttribute(CKA.ID));

            startDate = ReadAttribute(session, HObj, new DateAttribute((uint)CKA.START_DATE));

            endDate = ReadAttribute(session, HObj, new DateAttribute((uint)CKA.END_DATE));

            derive = ReadAttribute(session, HObj, new BooleanAttribute(CKA.DERIVE));

            local = ReadAttribute(session, HObj, new BooleanAttribute(CKA.LOCAL));

            keyGenMechanism = ReadAttribute(session, HObj, new MechanismTypeAttribute(CKA.KEY_GEN_MECHANISM));
        }
        
        public override string ToString()
        {
            // This method returns the best value.
            if (Label.Value != null)
            {
                return base.ToString();
            }
            else if (id.Value != null)
            {
                // Not bad, but could be better.
                return this.GetType().FullName + " " + id.ToString();
            }
            else
            {
                // Default handler.
                return base.ToString();
            }
        }
    }
}
