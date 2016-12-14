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

        [DllImport("eIDViewerBackend.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern int eid_vwr_set_cbfuncs(CbNewSrc theCbNewSrc, CbNewStringData theCbNewStringData,
            Cbnewbindata theCbnewbindata, Cblog theCbLog, Cbnewstate theCbnewstate, Cbpinop_result theCbpinopResult, CbReaders_changed theCbReadersChanged);

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

        //void(*readers_changed)(unsigned long nreaders, slotdesc* slots)
        public static void Init()
        {
            eid_vwr_set_cbfuncs(mynewsrc, mystringdata,
                mybindata, mylog, mynewstate, mypinopresult, myReadersChanged);

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

        private static void AdjustIconImage(string fileName)
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

        private static void CSCbNewSrc(eid_vwr_source eid_vwr_source)
        {
            if (theData.log_level == eid_vwr_loglevel.EID_VWR_LOG_DETAIL)
            {
                theData.logText += "CSCbNewSrc called " + eid_vwr_source.ToString() + "\n";
            }
            switch (eid_vwr_source)
            {
                case eid_vwr_source.EID_VWR_SRC_UNKNOWN:
                    theData.ResetDataValues();
                    AdjustIconImage("Resources/Images/state_error.png");
                    break;
                case eid_vwr_source.EID_VWR_SRC_NONE:
                    theData.ResetDataValues();
                    AdjustIconImage("Resources/Images/state_noeidpresent.png");
                    break;
                case eid_vwr_source.EID_VWR_SRC_CARD:
                    theData.text_color = "Black";
                    AdjustIconImage("Resources/Images/state_eidpresent.png");
                    theData.eid_card_present = true;
                    theData.progress_bar_visible = "Visible";
                    break;
                case eid_vwr_source.EID_VWR_SRC_FILE:
                    theData.text_color = "Black";
                    AdjustIconImage("Resources/Images/state_fileloaded.png");
                    theData.eid_card_present = true;
                    theData.progress_bar_visible = "Visible";
                    break;
                default:
                    break;
            }
        }
        private static void CSCbNewStringData([MarshalAs(UnmanagedType.LPWStr)] string label, [MarshalAs(UnmanagedType.LPWStr)]string data)
        {
            theData.StoreStringData(label, data);
            if (theData.log_level == eid_vwr_loglevel.EID_VWR_LOG_DETAIL)
            {
                theData.logText += "CSCbNewStringData called, data =  " + data + "\n";
            }
        }

        private static void CSCbnewbindata([MarshalAs(UnmanagedType.LPWStr)] string label, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] byte[] data, int datalen)
        {
            if (theData.log_level == eid_vwr_loglevel.EID_VWR_LOG_DETAIL)
            {
                theData.logText += "CSCbnewbindata called " + label + "\n";
            }
            if (data != null)
            {
                theData.StoreBinData(label, data, datalen);
            }
        }

        private static void CSCblog(eid_vwr_loglevel logLevel, [MarshalAs(UnmanagedType.LPWStr)]string str)
        {
            if (logLevel >= theData.log_level)
            {
                theData.logText += "CSCblogtest called " + str + "\n";
            }
        }

        private static void CSCbnewstate(eid_vwr_states state)
        {
            if (theData.log_level == eid_vwr_loglevel.EID_VWR_LOG_DETAIL)
            {
                theData.logText += "CSCbnewstate called " + state.ToString() + "\n";
            }
            switch (state)
            {
                case eid_vwr_states.STATE_TOKEN_WAIT:
                    theData.eid_data_from_file = false;
                    theData.VerifyAllData();
                    theData.HideProgressBar();
                    theData.print_enabled = true;
                    break;
                case eid_vwr_states.STATE_FILE_WAIT:
                    theData.eid_data_from_file = true;
                    theData.HideProgressBar();
                    break;
                case eid_vwr_states.STATE_READY:
                    theData.eid_data_ready = false;
                    theData.eid_data_from_file = false;
                    break;
                case eid_vwr_states.STATE_LIBOPEN:
                    theData.eid_data_ready = false;
                    theData.eid_data_from_file = false;
                    break;
                case eid_vwr_states.STATE_CARD_INVALID:
                    theData.ResetDataValues();
                    theData.eid_data_ready = false;
                    theData.eid_data_from_file = false;
                    AdjustIconImage("Resources/Images/state_error.png");
                    break;
                case eid_vwr_states.STATE_TOKEN_ERROR:
                    theData.ResetDataValues();
                    theData.eid_data_ready = false;
                    theData.eid_data_from_file = false;
                    break;
                case eid_vwr_states.STATE_FILE:
                    theData.eid_data_from_file = true;
                    break;
                default:
                    theData.eid_data_ready = true;
                    theData.eid_data_from_file = false;
                    break;
            }
        }

        private static void CSCbpinopResult(eid_vwr_pinops pinop, eid_vwr_result result)
        {
            try
            {
                if (theData.log_level == eid_vwr_loglevel.EID_VWR_LOG_DETAIL)
                {
                    theData.logText += "CSCbpinopResult called, result = " + result.ToString() + "\n";
                }

                ResourceManager rm = new ResourceManager("eIDViewer.Resources.ApplicationStringResources",
                    Assembly.GetExecutingAssembly());

                switch (result)
                {
                    //pkcs11 will bring up a message box in case of a failure
                    //case eid_vwr_result.EID_VWR_RES_FAILED:
                    //    System.Windows.MessageBox.Show("PinOp Failed");
                    //    break;
                    case eid_vwr_result.EID_VWR_RES_SUCCESS:
                        System.Windows.MessageBox.Show(rm.GetString("pinVerifiedOKDialogMessage", Thread.CurrentThread.CurrentUICulture));
                        break;
                }
            }
            catch (Exception e)
            {
                theData.logText += "CSCbpinopResult encountered an error " + e.ToString() + "\n";
            }
        }


        private static void CbReadersChanged(UInt32 nreaders, IntPtr slotList)
        {
            int structSize = Marshal.SizeOf(typeof(eid_slotdesc));
            Console.WriteLine(structSize);

            ConcurrentQueue<ReadersMenuViewModel> tempReadersList = new ConcurrentQueue<ReadersMenuViewModel>();
            // theData.readersList = new ConcurrentQueue<ReadersMenuViewModel>();

            if(nreaders == 0)
            {
                theData.logText += "No card readers detected\n";
            }

            for (int i = 0; i < nreaders; i++)
            {
                IntPtr data = new IntPtr(slotList.ToInt64() + structSize * i);
                eid_slotdesc slotDesc = (eid_slotdesc)Marshal.PtrToStructure(data, typeof(eid_slotdesc));

                theData.logText += "Reader slotnr  " + slotDesc.slot.ToString() + "\n";
                theData.logText += "Reader name  " + slotDesc.description.ToString() + "\n";

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

        public static void DoPinop(eid_vwr_pinops pinop)
        {
            eid_vwr_pinop(pinop);
        }

        public static void OpenXML(string sourceFile)
        {
            eid_vwr_be_deserialize(sourceFile);
        }

        public static void SaveXML(string destFile)
        {
            eid_vwr_be_serialize(destFile);
        }

        public static void ChangeLanguage(eid_vwr_langs language)
        {
            eid_vwr_convert_set_lang(language);
        }

        public static IntPtr GetXMLForm()
        {
            return eid_vwr_be_get_xmlform();
        }

        public static void SelectCardReader(int auto, UInt32 slotnr)
        {
            eid_vwr_be_select_slot(auto, slotnr);
        }

        public static void MarkCardInvalid()
        {
            eid_vwr_be_set_invalid();
        }

        public static void CloseXML()
        {
            eid_vwr_close_file();
        }


        // public CSCbStruct mCSCbStruct;
    }

}
