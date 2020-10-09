
using System;
using System.Windows.Threading;
using System.Runtime.InteropServices;
using System.Windows.Media.Imaging;
using System.Collections.ObjectModel;
using System.Collections.Concurrent;
using System.Resources;
using System.Reflection;
using System.Threading;
using System.Windows.Input;
using System.Globalization;
using System.Windows;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;


/*
[StructLayout(LayoutKind.Sequential)]
public struct CSCbStruct
{
    public CbNewSrc theCbNewSrc;
    public CbNewStringData theCbNewStringData;
    public Cbnewbindata theCbnewbindata;
    public Cblog theCbLog;
    public Cblog theCbLogv;
    public Cbnewstate theCbnewstate;
    public Cbpinop_result theCbpinopResult;
}*/

namespace eIDViewer
{

    public static class NativeMethods
    {
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void CbNewSrc(eid_vwr_source eid_vwr_source);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void CbNewStringData([MarshalAs(UnmanagedType.LPWStr)] string label, [MarshalAs(UnmanagedType.LPWStr)]string data);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void Cbnewbindata([MarshalAs(UnmanagedType.LPWStr)] string label, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] byte[] data, int datalen);
        //size param (int datalen) has index 2

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void Cblog(eid_vwr_loglevel logLevel, [MarshalAs(UnmanagedType.LPWStr)] string str);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void Cbnewstate(eid_vwr_states state);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void Cbpinop_result(eid_vwr_pinops pinop, eid_vwr_result result);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void CbsetLang(eid_vwr_langs lang);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void CbReaders_changed(UInt32 nreaders, IntPtr slotList);
        //private delegate void CbReaders_changed(UInt32 nreaders, eid_slotdesc[] slotList);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void Cbchallenge_result([MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] byte[] data, int datalen, eid_vwr_result result);
        //size param (int datalen) has index 1

        public static BackendDataViewModel theData { get; set; }

        //list all functions of the C backend we need to call
        //[DllImport("eIDViewerBackend.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        //public static extern int eid_vwr_createcallbacks(ref CSCbStruct eid_vwr_ui_callbacks);       
        private static Cblog mylog = NativeMethods.CSCblog;
        private static CbNewSrc mynewsrc = NativeMethods.CSCbNewSrc;
        private static CbNewStringData mystringdata = NativeMethods.CSCbNewStringData;
        private static Cbnewbindata mybindata = NativeMethods.CSCbnewbindata;
        private static Cbnewstate mynewstate = NativeMethods.CSCbnewstate;
        private static Cbpinop_result mypinopresult = NativeMethods.CSCbpinopResult;
        private static CbReaders_changed myReadersChanged = NativeMethods.CbReadersChanged;
        private static Cbchallenge_result mychallengeresult = NativeMethods.CSCbchallengeResult;

        [DllImport("eIDViewerBackend.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int eid_vwr_set_cbfuncs(CbNewSrc theCbNewSrc, CbNewStringData theCbNewStringData,
            Cbnewbindata theCbnewbindata, Cblog theCbLog, Cbnewstate theCbnewstate, Cbpinop_result theCbpinopResult,
            CbReaders_changed theCbReadersChanged, Cbchallenge_result thechallengeresult);

        [DllImport("eIDViewerBackend.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int eid_vwr_pinop(eid_vwr_pinops pinop);

        [DllImport("eIDViewerBackend.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int eid_vwr_be_deserialize([MarshalAs(UnmanagedType.LPWStr)] string source_file);

        [DllImport("eIDViewerBackend.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int eid_vwr_be_serialize([MarshalAs(UnmanagedType.LPWStr)] string dest_file);

        [DllImport("eIDViewerBackend.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int eid_vwr_convert_set_lang(eid_vwr_langs lang);

        [DllImport("eIDViewerBackend.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.SysInt)]
        private static extern IntPtr eid_vwr_be_get_xmlform();

        [DllImport("eIDViewerBackend.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void eid_vwr_be_select_slot([MarshalAs(UnmanagedType.I4)] int automatic, [MarshalAs(UnmanagedType.U4)] UInt32 manualslot);

        [DllImport("eIDViewerBackend.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void eid_vwr_be_set_invalid();

        [DllImport("eIDViewerBackend.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void eid_vwr_close_file();

        [DllImport("eIDViewerBackend.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int eid_vwr_challenge(byte[] challenge, [MarshalAs(UnmanagedType.I4)] int challengelen);

        //void(*readers_changed)(unsigned long nreaders, slotdesc* slots)
        public static void Init()
        {
            try
            {
                eid_vwr_set_cbfuncs(mynewsrc, mystringdata,
                    mybindata, mylog, mynewstate, mypinopresult, myReadersChanged, mychallengeresult);

                theData.cardreader_icon = new BitmapImage(new Uri("Resources/Images/state_noreaders.png", UriKind.Relative));
                /*
                //fill in the functions reference struct
                eIDViewerBackend.mCSCbStruct.theCbNewSrc = eIDViewerBackend.CSCbNewSrc;
                eIDViewerBackend.mCSCbStruct.theCbNewStringData = eIDViewerBackend.CSCbNewStringData;
                eIDViewerBackend.mCSCbStruct.theCbnewbindata = eIDViewerBackend.CSCbnewbindata;
                eIDViewerBackend.mCSCbStruct.theCbLog = eIDViewerBackend.CSCblog;
                eIDViewerBackend.mCSCbStruct.theCbLogv = null;
                eIDViewerBackend.mCSCbStruct.theCbnewstate = eIDViewerBackend.CSCbnewstate;
                eIDViewerBackend.mCSCbStruct.theCbpinopResult = eIDViewerBackend.CSCbpinopResult;

                var managedItem = (MMTPConxNack)Marshal.PtrToStructure(pointer,typeof(MMTPConxNack));

                eid_vwr_createcallbacks(ref mCSCbStruct);*/
            }
            catch (Exception e)
            {
                theData.WriteLog("AdjustIconImage encountered an error " + e.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
        }

        private static void AdjustIconImage(string fileName)
        {
            try
            {
                if (App.Current.Dispatcher != null)
                {
                    if (App.Current.Dispatcher.CheckAccess())
                    {
                        theData.cardreader_icon = new BitmapImage(new Uri(fileName, UriKind.Relative));
                    }
                    else
                    {
                        App.Current.Dispatcher.BeginInvoke(
                          DispatcherPriority.Background,
                          new Action(() =>
                          {
                              theData.cardreader_icon = new BitmapImage(new Uri(fileName, UriKind.Relative));
                          }));
                    }
                }
            }
            catch (Exception e)
            {
                theData.WriteLog("AdjustIconImage encountered an error " + e.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
        }

        private static void CSCbNewSrc(eid_vwr_source eid_vwr_source)
        {
            theData.WriteLog("CSCbNewSrc called " + eid_vwr_source.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);
            theData.ResetDataValues();
            switch (eid_vwr_source)
            {
                case eid_vwr_source.EID_VWR_SRC_UNKNOWN:            
                    AdjustIconImage("Resources/Images/state_error.png");
                    break;
                case eid_vwr_source.EID_VWR_SRC_NONE:
                    AdjustIconImage("Resources/Images/state_noeidpresent.png");
                    break;
                case eid_vwr_source.EID_VWR_SRC_CARD:
                    theData.open_enabled = false;
                    AdjustIconImage("Resources/Images/state_eidpresent.png");
                    theData.eid_card_present = true;
                    theData.progress_bar_visible = "Visible";
                    theData.progress_info = "reading eID card";
                    break;
                case eid_vwr_source.EID_VWR_SRC_FILE:
                    AdjustIconImage("Resources/Images/state_fileloaded.png");
                    theData.eid_card_present = true;
                    theData.progress_info = "reading eID file";
                    theData.progress_bar_visible = "Visible";
                    break;
                default:
                    break;
            }
        }
        private static void CSCbNewStringData([MarshalAs(UnmanagedType.LPWStr)] string label, [MarshalAs(UnmanagedType.LPWStr)]string data)
        {
            
            theData.WriteLog("CSCbNewStringData called, data =  " + data + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
            theData.StoreStringData(label, data);
        }

        private static void CSCbnewbindata([MarshalAs(UnmanagedType.LPWStr)] string label, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] byte[] data, int datalen)
        {
            theData.WriteLog("CSCbnewbindata called " + label + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);

            if (data != null)
            {
                theData.StoreBinData(label, data, datalen);
            }
        }

        private static void CSCblog(eid_vwr_loglevel logLevel, [MarshalAs(UnmanagedType.LPWStr)]string str)
        {
            theData.WriteLog("CSCblogtest called " + str + "\n", logLevel);          
        }

        private static void CSCbnewstate(eid_vwr_states state)
        {
            theData.WriteLog("CSCbnewstate called " + state.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);

            switch (state)
            {
                case eid_vwr_states.STATE_TOKEN_WAIT:
                    //Token has been read completely
                    theData.eid_data_from_file = false;
                    //VerifyAllData will check data integrity
                    //no data will be displayed, and eid_data_ready will be set to false when data integrity check fails
                    //eid_data_ready will be set to true when id and address data integrity check succeeds
                    theData.VerifyAllData();
                    theData.HideProgressBar();
                    break;
                case eid_vwr_states.STATE_FILE_WAIT:
                    //File has been read completely
                    theData.eid_data_from_file = true;
                    theData.eid_data_ready = true;
                    theData.HideProgressBar();
                    break;
                case eid_vwr_states.STATE_LIBOPEN:
                    //initial state of the eID Viewer Backend
                    theData.eid_read_data_started = false;
                    theData.eid_data_ready = false;
                    theData.eid_data_from_file = false;
                    break;
                case eid_vwr_states.STATE_CARD_INVALID:
                    theData.ResetDataValues();
                    theData.eid_data_ready = false;
                    theData.eid_read_data_started = false;
                    theData.eid_data_from_file = false;
                    AdjustIconImage("Resources/Images/state_error.png");
                    break;
                case eid_vwr_states.STATE_TOKEN_ERROR:
                    //substate of state_token 
                    theData.ResetDataValues();
                    theData.eid_read_data_started = false;
                    theData.eid_data_ready = false;
                    theData.eid_data_from_file = false;
                    break;
                case eid_vwr_states.STATE_FILE:
                    //file is present
                    theData.eid_read_data_started = true;
                    theData.eid_data_from_file = true;           
                    theData.open_enabled = true;
                    break;
                case eid_vwr_states.STATE_TOKEN:
                    //token is present
                    theData.eid_read_data_started = true;
                    theData.eid_data_ready = false;
                    theData.eid_data_from_file = false;
                    theData.open_enabled = false;
                    break;
                case eid_vwr_states.STATE_NO_TOKEN:
                    //no token, nor file is present
                    theData.eid_read_data_started = false;
                    theData.eid_data_ready = false;
                    theData.eid_data_from_file = false;
                    theData.open_enabled = true;
                    break;
                default:
                    break;
            }
            theData.eid_backend_state = state;
        }

        private static void CSCbpinopResult(eid_vwr_pinops pinop, eid_vwr_result result)
        {
            try
            {
                theData.WriteLog("CSCbpinopResult called, result = " + result.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);

                ResourceManager rm = new ResourceManager("eIDViewer.Resources.ApplicationStringResources",
                    Assembly.GetExecutingAssembly());

                

                switch (result)
                {
                    //pkcs11 will bring up a message box in case of a failure
                    //case eid_vwr_result.EID_VWR_RES_FAILED:
                    //    System.Windows.MessageBox.Show("PinOp Failed");
                    //    break;
                    case eid_vwr_result.EID_VWR_RES_SUCCESS:
                        if (pinop == eid_vwr_pinops.EID_VWR_PINOP_TEST)
                        {
                            theData.pincodeVerifiedSucces("pinVerifiedOKDialogMessage");
                        }
                        else if (pinop == eid_vwr_pinops.EID_VWR_PINOP_CHG)
                        {
                            theData.pincodeVerifiedSucces("pinChangedOKDialogMessage");
                        }
                        //CultureInfo culture = new CultureInfo(theData.localization);
                        // System.Windows.MessageBox.Show(rm.GetString("pinVerifiedOKDialogMessage", culture));
                        break;
                }
            }
            catch (Exception e)
            {
                theData.WriteLog("CSCbpinopResult encountered an error " + e.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
        }


        private static void CbReadersChanged(UInt32 nreaders, IntPtr slotList)
        {

            try
            {
                int structSize = Marshal.SizeOf(typeof(eid_slotdesc));
                ConcurrentQueue<ReadersMenuViewModel> tempReadersList = new ConcurrentQueue<ReadersMenuViewModel>();

                Console.WriteLine(structSize);
                // theData.readersList = new ConcurrentQueue<ReadersMenuViewModel>();

                if (nreaders == 0)
                {
                    theData.WriteLog("No card readers detected\n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);
                }

                for (int i = 0; i < nreaders; i++)
                {
                    IntPtr data = new IntPtr(slotList.ToInt64() + structSize * i);
                    eid_slotdesc slotDesc = (eid_slotdesc)Marshal.PtrToStructure(data, typeof(eid_slotdesc));

                    if (slotDesc.description == null)
                    {
                        theData.WriteLog("CbReadersChanged called without a reader description\n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);
                        break;
                    }

                    theData.WriteLog("Reader slotnr  " + slotDesc.slot.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);
                    theData.WriteLog("Reader name  " + slotDesc.description.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);

                    if (!slotDesc.description.Equals("\\\\?PnP?\\Notification"))
                    {
                        tempReadersList.Enqueue(new ReadersMenuViewModel(slotDesc.description, slotDesc.slot));
                    }
                    else if (nreaders == 1)
                    {
                        tempReadersList.Enqueue(new ReadersMenuViewModel(" ", 0));
                    }
                }
                theData.readersList = tempReadersList;
            }
            catch (Exception e)
            {
                theData.WriteLog("CbReadersChanged encountered an error " + e.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
        }

        private static void CSCbchallengeResult([MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] byte[] signature, int signaturelen, eid_vwr_result result)
        {
            try
            {
                theData.WriteLog("CSCbchallengeResult called, result = " + result.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);

                switch (result)
                {
                    //in case the function failed, we should not generate an error
                    case eid_vwr_result.EID_VWR_RES_SUCCESS:
                        //verify the response

                        /* Offset       ENCODING                                            ASN.1 Syntax
                         * 
                         *  00          30 76                                               -- SEQUENCE LENGTH
                            02                  30 10                                       -- SEQUENCE LENGTH
                                                                                            Label
                            04                          06 07                               -- OBJECT_ID LENGTH
                                                        2A 86 48 CE 3D 02 01                EcPublicKey (1 2 840 10045 2 1)
                            
                            0D                          06 05                               -- OBJECT_ID LENGTH
                            0F                          2B 81 04 00 22                      Secp384r1 (1 3 132 0 34)
                           
                            14                  03 62                                       -- BIT_STRING (98 bytes) LENGTH
                            16                          00                                  -- no bits unused in the final byte
                            17                          04                                  compression byte
                            18                          {48 bytes}                          -- X coordinate
                            48                          {48 bytes}                          -- Y coordinate
                         * */

                        //For now: No real parsing here, only accepting the above fixed format
                        //Will add the parsing in pkcs#11, or here, later
                        if(theData.basicKeyFile.Length != 0x78)
                        {
                            //File for supported format is incorrect, cannot verify, exit
                        }

                        byte[] KeyParams = new byte[5];
                        byte[] Secp384r1 = { 0x2B, 0x81, 0x04, 0x00, 0x22 };

                        byte[] KeyValue_X = new byte[48];
                        byte[] KeyValue_Y = new byte[48];

                        Array.Copy(theData.basicKeyFile, 0x0F, KeyParams, 0, 5);

                        ECParameters parameters = new ECParameters();
                        if (System.Collections.StructuralComparisons.StructuralEqualityComparer.Equals(KeyParams, Secp384r1))
                        {
                            //Fill in parameters named curve:
                            //Create a named curve using the specified Oid object.
                            System.Security.Cryptography.Oid cardP384oid = new Oid("ECDSA_P384");
                            parameters.Curve = ECCurve.CreateFromOid(cardP384oid);

                            Array.Copy(theData.basicKeyFile, 0x18, KeyValue_X, 0, 48);
                            Array.Copy(theData.basicKeyFile, 0x48, KeyValue_Y, 0, 48);

                            //Fill in parameters public key (Q)
                            System.Security.Cryptography.ECPoint Q;
                            Q.X = KeyValue_X;
                            Q.Y = KeyValue_Y;

                            parameters.Q = Q;
                        }
                        else
                        {
                            //not supported, cannot verify, exit
                        }

                        ECDsa dsa = ECDsa.Create(parameters);
                        if (dsa.VerifyData(theData.challenge, signature, HashAlgorithmName.SHA384))
                            Console.WriteLine("Data is good");
                        else
                            Console.WriteLine("Data is bad");

                        break;
                    case eid_vwr_result.EID_VWR_RES_FAILED:
                        //mark the verification as not happened
                        theData.WriteLog("CSCbchallengeResult encountered an error, key verification could not start \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                        break;
                }
            }
            catch (Exception e)
            {
                theData.WriteLog("CSCbchallengeResult encountered an error " + e.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
        }

        private static X509Certificate2 X509Certificate2(string v1, string v2)
        {
            throw new NotImplementedException();
        }

        public static void DoPinop(eid_vwr_pinops pinop)
        {            
            try
            {
                eid_vwr_pinop(pinop);
            }
            catch (Exception e)
            {
                MessageBox.Show("eid_vwr_pinop " + e.ToString() + "\n", "eID Viewer Backend Error");
                theData.WriteLog("eid_vwr_pinop " + e.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
        }

        public static int DoChallenge(byte[] challenge, int challengelen)
        {
            try
            {
                return eid_vwr_challenge(challenge, challengelen);
            }
            catch (Exception e)
            {
                MessageBox.Show("eid_vwr_challenge " + e.ToString() + "\n", "eID Viewer Backend Error");
                theData.WriteLog("eid_vwr_challenge " + e.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                return -1;
            }
        }

        public static void OpenXML(string sourceFile)
        {  
            try
            {
                eid_vwr_be_deserialize(sourceFile);
            }
            catch (Exception e)
            {
                MessageBox.Show("eid_vwr_be_deserialize " + e.ToString() + "\n", "eID Viewer Backend Error");
                theData.WriteLog("eid_vwr_be_deserialize " + e.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
        }

        public static void SaveXML(string destFile)
        {        
            try
            {
                eid_vwr_be_serialize(destFile);
            }
            catch (Exception e)
            {
                MessageBox.Show("eid_vwr_be_serialize " + e.ToString() + "\n", "eID Viewer Backend Error");
                theData.WriteLog("eid_vwr_be_serialize " + e.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
        }

        public static void ChangeLanguage(eid_vwr_langs language)
        {
            try
            { 
                eid_vwr_convert_set_lang(language);
            }
            catch (System.DllNotFoundException e)
            {
                String FAQ_url_updatecruntime = "https://eid.belgium.be/en/technical-documentation#7483";
                CultureInfo culture = Thread.CurrentThread.CurrentCulture;
                if (culture.TwoLetterISOLanguageName.Equals("nl")){
                    FAQ_url_updatecruntime = "https://eid.belgium.be/nl/technical-documentation#7483";
                } else
                if (culture.TwoLetterISOLanguageName.Equals("fr"))
                {
                    FAQ_url_updatecruntime = "https://eid.belgium.be/fr/technical-documentation#7483";
                } else
                if (culture.TwoLetterISOLanguageName.Equals("de"))
                {
                    FAQ_url_updatecruntime = "https://eid.belgium.be/de/technical-documentation#7483";
                }

                MessageBox.Show("eid_vwr_convert_set_lang" + e.ToString() + "\n" + "\n" + "Info: " + FAQ_url_updatecruntime + "\n", "eID Viewer Backend Error");
                theData.WriteLog("eid_vwr_convert_set_lang" + e.ToString() + "\n" + "\n" + "Info: " + FAQ_url_updatecruntime + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);

                /*theData.WriteLog("eid_vwr_convert_set_lang" + e.ToString() + "\n" + "Is your Windows up to date?" + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                theData.WriteLog("In case your Windows 7, 8 or 8.1 is missing the universal c runtime in Windows: \n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                theData.WriteLog("https://support.microsoft.com/en-us/help/2999226/update-for-universal-c-runtime-in-windows" + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);       */
            }
            catch (Exception e)
            {
                MessageBox.Show("eid_vwr_convert_set_lang" + e.ToString() + "\n", "eID Viewer Backend Error");
                theData.WriteLog("eid_vwr_convert_set_lang" + e.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
        }

        public static IntPtr GetXMLForm()
        {       
            try
            {
                return eid_vwr_be_get_xmlform();
            }
            catch (Exception e)
            {
                MessageBox.Show("eid_vwr_be_get_xmlform " + e.ToString() + "\n", "eID Viewer Backend Error");
                theData.WriteLog("eid_vwr_be_get_xmlform " + e.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
                throw;
            }
        }

        public static void SelectCardReader(int auto, UInt32 slotnr)
        {
            try
            {
                eid_vwr_be_select_slot(auto, slotnr);
            }
            catch (Exception e)
            {
                MessageBox.Show("eid_vwr_be_select_slot " + e.ToString() + "\n", "eID Viewer Backend Error");
                theData.WriteLog("eid_vwr_be_select_slot " + e.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            } 
        }

        public static void MarkCardInvalid()
        {        
            try
            {
                eid_vwr_be_set_invalid();
            }
            catch (Exception e)
            {
                MessageBox.Show("eid_vwr_be_set_invalid " + e.ToString() + "\n", "eID Viewer Backend Error");
                theData.WriteLog("eid_vwr_be_set_invalid " + e.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
        }

        public static void CloseXML()
        {     
            try
            {
                eid_vwr_close_file();
            }
            catch (Exception e)
            {
                MessageBox.Show("eid_vwr_close_file " + e.ToString() + "\n", "eID Viewer Backend Error");
                theData.WriteLog("eid_vwr_close_file " + e.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_ERROR);
            }
        }


        // public CSCbStruct mCSCbStruct;
    }

}
