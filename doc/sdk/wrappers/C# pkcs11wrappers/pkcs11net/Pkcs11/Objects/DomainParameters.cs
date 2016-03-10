using System;
using System.Collections.Generic;
using System.Text;

namespace Net.Sf.Pkcs11.Objects
{

    /// <summary>
    /// Attribute - Data type - Meaning
    /// CKA_PRIME1,4 Big integer Prime p (512 to 1024 bits, in steps of 64 bits)
    /// CKA_SUBPRIME1,4 Big integer Subprime q (160 bits)
    /// CKA_BASE1,4 Big integer Base g
    /// CKA_PRIME_BITS2,3 CK_ULONG Length of the prime value.
    /// </summary>
    public class DomainParameters : Storage
    {
        public static new P11Object GetInstance(Session session, uint hObj)
        {
            return new DomainParameters(session, hObj);
        }

        public DomainParameters(Session session, uint hObj)
            : base(session, hObj)
        {
        }

        public DomainParameters()
            : base()
        {

        }
    }
}
