
using System;
using System.Collections;
using System.Collections.Generic;
using Net.Sf.Pkcs11.Wrapper;

namespace Net.Sf.Pkcs11
{
    /// <summary>
    /// Wrapper around Pkcs11(Cryptoki) module (high-level).
    /// </summary>
    public class Module : IDisposable
    {
        #region Members

        protected Pkcs11.Wrapper.Pkcs11Module p11Module;

        #endregion

        #region Properties

        /// <summary>
        /// Pkcs11Module property for low-level access
        /// </summary>
        public Pkcs11Module P11Module
        {
            get { return p11Module; }
        }

        #endregion

        #region Methods

        #region Instance

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="p11Module">Pkcs11Module core object</param>
        protected Module(Pkcs11Module p11Module)
        {
            this.p11Module = p11Module;
            Initialize();
        }

        /// <summary>
        /// Creates an instance of Pkcs11Module
        /// </summary>
        /// <param name="moduleName">
        /// module to be loaded. it is the path of pkcs11 driver
        /// <example>
        /// <code>
        /// Pkcs11Module pm=Pkcs11Module.GetInstance("gclib.dll");
        /// </code>
        /// </example>
        /// </param>
        /// <returns></returns>				
        public static Module GetInstance(String moduleName)
        {
            if (moduleName == null)
            {
                throw new Exception("Argument \"pkcs11ModuleName\" must not be null.");
            }
            else
            {
                Pkcs11.Wrapper.Pkcs11Module pm = Pkcs11.Wrapper.Pkcs11Module.GetInstance(moduleName);

                return new Module(pm);
            }
        }

        #endregion

        #region Cryptoki Module Information
        public Info GetInfo()
        {
            CK_INFO localCK_INFO = this.p11Module.GetInfo();

            return new Info(localCK_INFO);
        }
        
        #endregion        
        
        #region Slots

        /// <summary>
        /// Get an Array of Slots.
        /// </summary>
        /// <param name="onlyTokenPresent">true if you wish to get only slots with Token. false otherwise</param>
        /// <returns>the Slots</returns>
        public Slot[] GetSlotList(bool onlyTokenPresent)
        {
            var slotIds = p11Module.GetSlotList(onlyTokenPresent);
            var slots = new List<Slot>();

            foreach (var slotId in slotIds)
            {
                var slot = new Slot(this, slotId);
                slots.Add(slot);
            }

            return (slots.ToArray());
        }

        /// <summary>
        /// Waits for a slot event to occur.
        /// </summary>
        /// <param name="options"></param>
        /// <returns></returns>
        public Slot WaitForSlotEvent(bool DO_NOT_BLOCK)
        {
            uint lSlotId = p11Module.WaitForSlotEvent(DO_NOT_BLOCK);
            return new Slot(this, lSlotId);
        }

        #endregion
       
        #region General

        private void Initialize()
        {
            p11Module.Initialize();
        }

        private void Finalize_()
        {
            p11Module.Finalize_();
        }

        public void Dispose()
        {
            Finalize_();
        }

        #endregion

        #endregion
    }
}
