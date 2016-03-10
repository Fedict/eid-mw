
using System;
using Net.Sf.Pkcs11.Wrapper;

namespace Net.Sf.Pkcs11.Params
{
    /// <summary>
    /// Description of custom Parameters.
    /// </summary>
    public class Parameters
    {
        /// <summary>
        /// Fill CK_MECHANISM according to values of Parameters.
        /// Default implementation fills everything with zeros.
        /// </summary>
        /// <param name="ckMechanism"></param>
        public virtual void ApplyToMechanism(CK_MECHANISM ckMechanism)
        {
            ckMechanism.pParameter = IntPtr.Zero;
            ckMechanism.ulParameterLen = 0;
        }

        /// <summary>
        /// Obtain a parameter class here.
        /// </summary>
        /// <param name="ckm"></param>
        /// <returns></returns>
        public static Parameters GetParameters(CKM ckm)
        {
            Parameters lParameters = new Parameters();
            return lParameters;
        }
    }
}
