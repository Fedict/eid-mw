// belpicgui.cpp : Defines the entry point for the DLL application.
//

#ifdef _WIN32
    #include <windows.h>
    #include "belpicgui.h"
    #include "PINDialog.h"
    #include "MessageDialog.h"
#endif
#ifdef __APPLE__
#include <CoreFoundation/CFBundle.h>
#endif

#include <qapplication.h>
#include <qprocess.h>
#include <qwaitcondition.h> 
#include "scgui.h"

#include "authenticationdialogimpl.h"
#include "changepindialogimpl.h"
#include "askmessagedialogimpl.h"
#include "insertcarddialogimpl.h"

#include <memory>

#if defined(_WIN32) || defined(__APPLE__)
std::auto_ptr<QApplication> gMyApp;
#endif

#ifdef _WIN32
    CAppModule _Module;

    BOOL WINAPI DllMain( HINSTANCE hModule, 
                           DWORD  ul_reason_for_call, 
                           LPVOID lpReserved
					     )
    {
        switch (ul_reason_for_call)
	    {
		    case DLL_PROCESS_ATTACH:
            {
	            HRESULT hRes = ::CoInitialize(NULL);
	            // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	            ::DefWindowProc(NULL, 0, 0, 0L);
	            AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls
	            hRes = _Module.Init(NULL, hModule);
                break;
            }
		    case DLL_THREAD_ATTACH:
                break;
		    case DLL_THREAD_DETACH:
                break;
		    case DLL_PROCESS_DETACH:
	            _Module.Term();
	            ::CoUninitialize();
			    break;
        }
        return TRUE;
    }
#endif

/************************/
/* SCGUI API functions */
/************************/
#if defined(_WIN32) || defined(__APPLE__)
SCGUI_API scgui_ret_t scgui_init(void)
{
    if(!qApp)
    {
#ifdef __APPLE__
	static char app_path[300];

        CFURLRef pluginRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
        CFStringRef macPath = CFURLCopyFileSystemPath(pluginRef, 
                                               kCFURLPOSIXPathStyle);
        const char *pathPtr = CFStringGetCStringPtr(macPath, 
                                               CFStringGetSystemEncoding());
	strcpy(app_path, pathPtr);
        CFRelease(pluginRef);
        CFRelease(macPath);
	static char *s = app_path;
#else
        static char *s = "belpicgui";
#endif
        int i = 1;
        std::auto_ptr<QApplication> app(new QApplication(i, &s)); 
        gMyApp = app;
    }
	return SCGUI_OK;
}
#else

static int i = 1;
static char *s = "belpicgui";
static QApplication gMyApp(i, &s);

SCGUI_API scgui_ret_t scgui_init(void)
{
	return SCGUI_OK;
}
#endif

SCGUI_API scgui_ret_t scgui_enterpin(const char *title, const char *msg, char *pin, int *len, const char *btn_ok, const char *btn_cancel, const char *pinTooShort, scgui_param_t iconSign)
{
    if(pin == NULL || len == 0 || *len == 0)
    {
        return SCGUI_BAD_PARAM;
    }
    if(*len < BELPIC_PIN_MAX_LENGTH + 1)
    {
        return SCGUI_BUFFER_TOO_SMALL;
    }

    VerifyPinData data = {0};

	data.msg = (char *)msg;
	data.pin[0] = '\0';
	data.btn_ok = (char *)btn_ok;
	data.btn_cancel = (char *)btn_cancel;
	data.title = (char *)title;
	data.pinTooShort = (char *)pinTooShort;
	data.iconSign = iconSign;

#ifdef _WIN32
    CPINDialog oDialog(&data);
    oDialog.DoModal();
#else
    authenticationdialogImpl oDialog(&data);
    oDialog.exec();
#endif
	*len = strlen(data.pin);
	if (*len > 0) 
    {
	    strcpy(pin, data.pin);
        memset(data.pin, 0, *len);
		return SCGUI_OK;
	}
	return SCGUI_CANCEL;
}

SCGUI_API scgui_ret_t scgui_changepin(const char *title, const char *msg, char *oldpin, int *oldpinlen, char *newpin, int *newpinlen, const char *btn_ok, const char *btn_cancel, const char *pinTooShort, const char *confirmPinError)
{
    if(oldpin == NULL || oldpinlen == 0 || *oldpinlen == 0)
    {
        return SCGUI_BAD_PARAM;
    }
    if(newpin == NULL || newpinlen == 0 || *newpinlen == 0)
    {
        return SCGUI_BAD_PARAM;
    }
    if(*oldpinlen < BELPIC_PIN_MAX_LENGTH + 1 || *newpinlen < BELPIC_PIN_MAX_LENGTH + 1)
    {
        return SCGUI_BUFFER_TOO_SMALL;
    }

    ChangePinData data = {0};

	data.msg = (char *)msg;
	data.oldpin[0] = data.newpin[0] = data.confirmpin[0] = '\0';
	data.btn_ok = (char *)btn_ok;
	data.btn_cancel = (char *)btn_cancel;
	data.title = (char *)title;
	data.pinTooShort = (char *)pinTooShort;
	data.confirmPinError = (char *)confirmPinError;

    changepindialogImpl oDialog(&data);
    oDialog.exec();

    *newpinlen = strlen(data.newpin);
	*oldpinlen = strlen(data.oldpin);

    if (*newpinlen > 0 && *oldpinlen > 0) 
    {
	    strcpy(oldpin, data.oldpin);
        memset(data.oldpin, 0, *oldpinlen);
	    strcpy(newpin, data.newpin);
        memset(data.newpin, 0, *newpinlen);
		return SCGUI_OK;
	}

	return SCGUI_CANCEL;
}

SCGUI_API scgui_ret_t scgui_display_message(const char *title, const char *short_msg, const char *long_msg, const char *btn_close, void **handle, scgui_param_t signIcon, const char *pinpad_reader)
{
    if(handle == NULL)
    {
        return SCGUI_BAD_PARAM;
    }
    *handle = NULL;

    QProcess *proc = new QProcess( );
    proc->addArgument( "shbeidgui" );
    proc->addArgument( title );
    proc->addArgument( short_msg );
    proc->addArgument( long_msg );
    proc->addArgument( btn_close ? btn_close : "");
    proc->addArgument( pinpad_reader );
    proc->addArgument( signIcon == SCGUI_SIGN_ICON ? "Yes" : "No" );

    if ( proc->start() ) 
    {
        *handle = proc;
        QWaitCondition oWait;
        oWait.wait(300);
    }
	return SCGUI_OK;
}

SCGUI_API scgui_ret_t scgui_remove_message(void *handle)
{
    if(handle != NULL)
    {
        ((QProcess *)handle)->tryTerminate();
        int iCount = 0;
        QWaitCondition oWait;
        while(((QProcess *)handle)->isRunning() && iCount < 30)
        {
            oWait.wait(100);
            iCount++;    
        }
    }
	return SCGUI_OK;
}

SCGUI_API scgui_ret_t scgui_ask_message(const char *title, const char *short_msg, const char *long_msg, const char *btn_ok, const char *btn_cancel, const char *pinpad_reader)
{
	AskMessageData data;
	data.short_msg = (char *)short_msg;
	data.long_msg = (char*)long_msg;
	data.ret = SCGUI_CANCEL;
	data.btn_ok = (char *)btn_ok;
	data.btn_cancel = (char *)btn_cancel;
	data.title = (char *)title;
	data.pinpad_reader = (char *)pinpad_reader;

#ifdef _WIN32
    CMessageDialog oDialog(&data);
    oDialog.DoModal();
#else
    askmessagedialogImpl oDialog(&data);
    oDialog.exec();
#endif
	return data.ret;
}

SCGUI_API scgui_ret_t scgui_insertcard(const char *title, const char *msg, const char *btn_ok, const char *btn_cancel, TimerFunc pFunc)
{
	InsertCardData data;
	data.msg = (char *)msg;
	data.ret = SCGUI_CANCEL;
	data.btn_ok = (char *)btn_ok;
	data.btn_cancel = (char *)btn_cancel;
	data.title = (char *)title;
    data.timer_func = pFunc;

    InsertCardDialogImpl oDialog(&data);
    oDialog.exec();

	return data.ret;
}

int MapLanguage()
{
#ifdef _WIN32
	switch (GetUserDefaultLangID() & 0x00FF) 
    {
	case 0x13: return 1;
	case 0x0C: return 2;
	case 0x07: return 3;
	default: return 0;
	}
#else
    return 0;
#endif
}
