/***************************************************************************
**
** Copyright (C) 2003-2005 Trolltech AS.  All rights reserved.
**
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Solutions License Agreement provided
** with the Solution.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE.
**
** Please email sales@trolltech.com for information
** about Qt Solutions License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
*/
#include "qtservice.h"
#include <qapplication.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qlibrary.h>
#include <qprocess.h>
#include <qsettings.h>
#include <qtextstream.h>
#include <qt_windows.h>
#include <stdio.h>

#ifdef WIN32
    #pragma warning(disable:4786 4018)
#endif

class QtServiceSysPrivate
{
public:
    QtServiceSysPrivate(QtService *that);

    void setStatus( DWORD dwState );
    inline bool available() const;
    static void WINAPI serviceMain( DWORD dwArgc, TCHAR** lpszArgv );
    static void WINAPI handler( DWORD dwOpcode );
    static DWORD WINAPI handlerEx( DWORD dwOpcode, DWORD dwEventType, LPVOID lpEventData,  LPVOID lpContext );
    bool findWindow();

    SERVICE_STATUS status;
    SERVICE_STATUS_HANDLE serviceStatus;
    QString filepath;

    DWORD procid;
    HWND messages;
    UINT WM_QTCOMMAND;
    static int QtServiceEventFilter(MSG *pMsg);
};

typedef SERVICE_STATUS_HANDLE(WINAPI*PRegisterServiceCtrlHandler)(LPCTSTR,LPHANDLER_FUNCTION);
static PRegisterServiceCtrlHandler pRegisterServiceCtrlHandler = 0;
typedef SERVICE_STATUS_HANDLE(WINAPI*PRegisterServiceCtrlHandlerEx)(LPCTSTR,LPHANDLER_FUNCTION_EX,LPVOID);
static PRegisterServiceCtrlHandlerEx pRegisterServiceCtrlHandlerEx = 0;
typedef BOOL(WINAPI*PSetServiceStatus)(SERVICE_STATUS_HANDLE,LPSERVICE_STATUS);
static PSetServiceStatus pSetServiceStatus = 0;
typedef BOOL(WINAPI*PChangeServiceConfig2)(SC_HANDLE,DWORD,LPVOID);
static PChangeServiceConfig2 pChangeServiceConfig2 = 0;
typedef BOOL(WINAPI*PCloseServiceHandle)(SC_HANDLE);
static PCloseServiceHandle pCloseServiceHandle = 0;
typedef SC_HANDLE(WINAPI*PCreateService)(SC_HANDLE,LPCTSTR,LPCTSTR,DWORD,DWORD,DWORD,DWORD,LPCTSTR,LPCTSTR,LPDWORD,LPCTSTR,LPCTSTR,LPCTSTR);
static PCreateService pCreateService = 0;
typedef SC_HANDLE(WINAPI*POpenSCManager)(LPCTSTR,LPCTSTR,DWORD);
static POpenSCManager pOpenSCManager = 0;
typedef BOOL(WINAPI*PDeleteService)(SC_HANDLE);
static PDeleteService pDeleteService = 0;
typedef SC_HANDLE(WINAPI*POpenService)(SC_HANDLE,LPCTSTR,DWORD);
static POpenService pOpenService = 0;
typedef BOOL(WINAPI*PQueryServiceStatus)(SC_HANDLE,LPSERVICE_STATUS);
static PQueryServiceStatus pQueryServiceStatus = 0;
typedef BOOL(WINAPI*PStartServiceCtrlDispatcher)(CONST SERVICE_TABLE_ENTRY*);
static PStartServiceCtrlDispatcher pStartServiceCtrlDispatcher = 0;
typedef BOOL(WINAPI*PStartService)(SC_HANDLE,DWORD,const char**);
static PStartService pStartService = 0;
typedef BOOL(WINAPI*PControlService)(SC_HANDLE,DWORD,LPSERVICE_STATUS);
static PControlService pControlService = 0;
typedef HANDLE(WINAPI*PDeregisterEventSource)(HANDLE);
static PDeregisterEventSource pDeregisterEventSource = 0;
typedef BOOL(WINAPI*PReportEvent)(HANDLE,WORD,WORD,DWORD,PSID,WORD,DWORD,LPCTSTR*,LPVOID);
static PReportEvent pReportEvent = 0;
typedef HANDLE(WINAPI*PRegisterEventSource)(LPCTSTR,LPCTSTR);
static PRegisterEventSource pRegisterEventSource = 0;
typedef DWORD(*PRegisterServiceProcess)(DWORD,DWORD);
static PRegisterServiceProcess pRegisterServiceProcess = 0;

#define RESOLVE(name) p##name = (P##name)lib.resolve(#name);
#define RESOLVEA(name) p##name = (P##name)lib.resolve(#name"A");
#define RESOLVEW(name) p##name = (P##name)lib.resolve(#name"W");

QtServiceSysPrivate::QtServiceSysPrivate(QtService *that)
:procid(0),messages(0)
{
    QString csName = that->serviceName();
    QT_WA({
        wchar_t path[_MAX_PATH];
	::GetModuleFileNameW( 0, path, sizeof(path) );
	filepath = QString::fromUcs2((unsigned short*)path);
	WM_QTCOMMAND = RegisterWindowMessageW((TCHAR*)csName.ucs2());
    }, {
	char path[_MAX_PATH];
	::GetModuleFileNameA( 0, path, sizeof(path) );
	filepath = QString::fromLocal8Bit(path);
	WM_QTCOMMAND = RegisterWindowMessageA(csName.local8Bit());
    });

    if (qWinVersion() & Qt::WV_DOS_based) {
	if (!pRegisterServiceProcess) {
	    QLibrary lib("kernel32");
	    lib.setAutoUnload(FALSE);
	    RESOLVE(RegisterServiceProcess);
	}
	return;
    }
    if (pOpenSCManager)
	return;

    QLibrary lib("advapi32");
    lib.setAutoUnload(FALSE);

    // only resolve unicode versions
    RESOLVEW(RegisterServiceCtrlHandler);
    RESOLVEW(RegisterServiceCtrlHandlerEx);
    RESOLVE(SetServiceStatus);
    RESOLVEW(ChangeServiceConfig2);
    RESOLVE(CloseServiceHandle);
    RESOLVEW(CreateService);
    RESOLVEW(OpenSCManager);
    RESOLVE(DeleteService);
    RESOLVEW(OpenService);
    RESOLVE(QueryServiceStatus);
    RESOLVEW(StartServiceCtrlDispatcher);
    RESOLVEA(StartService); // need only Ansi version
    RESOLVE(ControlService);
    RESOLVE(DeregisterEventSource);
    RESOLVEW(ReportEvent);
    RESOLVEW(RegisterEventSource);

    Q_ASSERT(available());
}

inline bool QtServiceSysPrivate::available() const
{
    return 0 != pOpenSCManager;
}

typedef int (*QWinEventFilter) (MSG*);
extern Q_EXPORT QWinEventFilter qt_set_win_event_filter(QWinEventFilter);
static QWinEventFilter oldEventFilter;
int QtServiceSysPrivate::QtServiceEventFilter( MSG *pMsg )
{
    if (qService) {
        if (pMsg->message == WM_ENDSESSION)
            return true;
     
        QtServiceSysPrivate *data = qService->sysd;
        if (pMsg->message == data->WM_QTCOMMAND)
        {
            handler(pMsg->wParam);
        }
    }

    return oldEventFilter ? oldEventFilter(pMsg) : false;
}

void WINAPI QtServiceSysPrivate::serviceMain( DWORD dwArgc, TCHAR** lpszArgv )
{
    if ( !qService )
	return;
    QtServiceSysPrivate *data = qService->sysd;
    QString csName = qService->serviceName();

    // Register the control request handler
    if(pRegisterServiceCtrlHandlerEx)
    {
        data->serviceStatus = pRegisterServiceCtrlHandlerEx( (TCHAR*)csName.ucs2(), handlerEx, NULL );
    }
    else
    {
        data->serviceStatus = pRegisterServiceCtrlHandler( (TCHAR *)csName.ucs2(), handler );
    }
    if ( !data->serviceStatus )
    {
	    return;
    }

    // Start the initialisation
    data->setStatus(SERVICE_START_PENDING);
    if ( qService->initialize() ) 
    {
	    data->setStatus(SERVICE_RUNNING);
	    // Do the real work. 
	    data->status.dwWin32ExitCode = NO_ERROR;
	    data->status.dwCheckPoint = 0;
	    data->status.dwWaitHint = 0;

	    int argc = dwArgc;
	    char **argv = new char*[ argc ];
	    for ( int i = 0; i < argc; i++ ) 
        {
	        QString a = QString::fromUcs2( (unsigned short*)lpszArgv[i] );
	        if ( !i )
		    a = data->filepath;
	        argv[i] = new char[ a.local8Bit().length() + 1 ];
	        strcpy( argv[i], a.local8Bit().data() );
	    }

        oldEventFilter = qt_set_win_event_filter(QtServiceEventFilter);
	    data->status.dwWin32ExitCode = qService->run( argc, argv );
	    delete []*argv;
    }

    // Tell the service manager we are stopped
    data->setStatus(SERVICE_STOPPED);
}

void WINAPI QtServiceSysPrivate::handler( DWORD code )
{
    if ( !qService )
	return;
    QtServiceSysPrivate *data = qService->sysd;

    switch (code) {
    case SERVICE_CONTROL_STOP: // 1
	data->setStatus( SERVICE_STOP_PENDING );
	qService->stop();
    data->setStatus(SERVICE_STOPPED);
	if ( qApp )
	    qApp->wakeUpGuiThread();
	break;

    case SERVICE_CONTROL_PAUSE: // 2
	data->setStatus( SERVICE_PAUSE_PENDING );
	qService->pause();
	data->setStatus( SERVICE_PAUSED );
	break;

    case SERVICE_CONTROL_CONTINUE: // 3
	data->setStatus( SERVICE_CONTINUE_PENDING );
	qService->resume();
	data->setStatus( SERVICE_RUNNING );
	break;

    case SERVICE_CONTROL_INTERROGATE: // 4
	break;

    default:
	if ( code >= 128 && code <= 255 )
	    qService->user( code - 128 );
	break;
    }

    // Report current status
    if ( data->available() )
	pSetServiceStatus( data->serviceStatus, &data->status );
}

DWORD WINAPI QtServiceSysPrivate::handlerEx( DWORD code, DWORD dwEventType, LPVOID lpEventData,  LPVOID lpContext )
{
    DWORD dwRet = NO_ERROR;
    if ( !qService )
	return dwRet;
    QtServiceSysPrivate *data = qService->sysd;

    switch (code) {
    case SERVICE_CONTROL_STOP: // 1
    case SERVICE_CONTROL_SHUTDOWN:
	data->setStatus( SERVICE_STOP_PENDING );
	qService->stop();
    data->setStatus(SERVICE_STOPPED);
	if ( qApp )
	    qApp->wakeUpGuiThread();
	break;

    case SERVICE_CONTROL_PAUSE: // 2
	data->setStatus( SERVICE_PAUSE_PENDING );
	qService->pause();
	data->setStatus( SERVICE_PAUSED );
	break;

    case SERVICE_CONTROL_CONTINUE: // 3
	data->setStatus( SERVICE_CONTINUE_PENDING );
	qService->resume();
	data->setStatus( SERVICE_RUNNING );
	break;

    case SERVICE_CONTROL_INTERROGATE: // 4
	break;

    case SERVICE_CONTROL_POWEREVENT:
    qService->powerEvent(dwEventType, lpEventData, lpContext);
    break;

    default:
	if ( code >= 128 && code <= 255 )
	    qService->user( code - 128 );
	break;
    }

    // Report current status
    if ( data->available() )
	pSetServiceStatus( data->serviceStatus, &data->status );

    return dwRet;
}

void QtServiceSysPrivate::setStatus( DWORD state )
{
    if ( !available() )
	return;
    status.dwCurrentState = state;
    pSetServiceStatus( serviceStatus, &status);
}

static BOOL CALLBACK qt_findwindow( HWND hwnd, LPARAM lParam )
{
    DWORD pid;
    QtServiceSysPrivate *sysd = (QtServiceSysPrivate*)lParam;
    GetWindowThreadProcessId( hwnd, &pid );
    if( pid == sysd->procid ) {
	sysd->messages = hwnd;
	return FALSE;
    }
    return TRUE;
}

bool QtServiceSysPrivate::findWindow()
{
    messages = 0;
    EnumWindows(qt_findwindow, (LPARAM)this);
    return messages != 0;
}

void QtService::sysInit()
{
    sysd = new QtServiceSysPrivate(this);

    sysd->serviceStatus			    = 0;
    sysd->status.dwServiceType		    = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
    sysd->status.dwCurrentState		    = SERVICE_STOPPED;
    sysd->status.dwControlsAccepted	    = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    if(pRegisterServiceCtrlHandlerEx)
    {
        sysd->status.dwControlsAccepted |= SERVICE_ACCEPT_POWEREVENT;
    }
    sysd->status.dwWin32ExitCode	    = NO_ERROR;
    sysd->status.dwServiceSpecificExitCode  = 0;
    sysd->status.dwCheckPoint		    = 0;
    sysd->status.dwWaitHint		    = 0;
}

void QtService::sysCleanup()
{
    delete sysd;
    sysd = 0;
}

bool QtService::install()
{
    if ( !sysd->available() ) {
	QSettings settings;
	settings.setPath("Microsoft", "Windows");
	bool res = FALSE;
	if ( startupType() != Auto ) {
	    settings.removeEntry("/CurrentVersion/RunServices/" + serviceName());
	    res = settings.writeEntry("/CurrentVersion/RunServicesManually/" + serviceName(), sysd->filepath);
	} else {
	    settings.removeEntry("/CurrentVersion/RunServicesManually/" + serviceName());
	    res = settings.writeEntry("/CurrentVersion/RunServices/" + serviceName(), sysd->filepath);
	}
	if ( !res )
	    reportEvent( "Installing the service failed", Error );
	else
	    reportEvent( "Service installed" );
	return res;
    }
    // Open the Service Control Manager
    SC_HANDLE hSCM = pOpenSCManager( 0, 0, SC_MANAGER_ALL_ACCESS );
    if (!hSCM)
	return FALSE;

    DWORD dwStartType;
    switch ( startupType() ) {
    case Auto:
	dwStartType = SERVICE_AUTO_START;
	break;
    default:
	dwStartType = SERVICE_DEMAND_START;
	break;
    }

    // Create the service
    SC_HANDLE hService = 0;
    QString csName = serviceName();
    QString csPath = sysd->filepath;
    hService = pCreateService( hSCM, (TCHAR*)csName.ucs2(), (TCHAR*)csName.ucs2(),
			       SERVICE_ALL_ACCESS, sysd->status.dwServiceType, 
			       dwStartType, SERVICE_ERROR_NORMAL, (TCHAR*)csPath.ucs2(), 
			       0, 0, 0, 0, 0 );
    if (!hService) {
	pCloseServiceHandle(hSCM);
	reportEvent( "Installing the service failed", Error );
	return FALSE;
    }
    if ( !serviceDescription().isNull() && pChangeServiceConfig2) {
	SERVICE_DESCRIPTION sdesc;
	QString cs = serviceDescription();
	sdesc.lpDescription = (TCHAR*)cs.ucs2();
	pChangeServiceConfig2( hService, SERVICE_CONFIG_DESCRIPTION, &sdesc );
    }

    // tidy up
    pCloseServiceHandle(hService);
    pCloseServiceHandle(hSCM);
    reportEvent( "Service installed" );
    return TRUE;
}

bool QtService::uninstall()
{
    bool result = FALSE;
    if (!sysd->available()) {
	QSettings settings;
	settings.setPath("Microsoft", "Windows");
	result = settings.removeEntry("/CurrentVersion/RunServices/" + serviceName());
	result = settings.removeEntry("/CurrentVersion/RunServicesManually/" + serviceName());
    } else {
	// Open the Service Control Manager
	SC_HANDLE hSCM = 0;
	hSCM = pOpenSCManager( 0, 0, SC_MANAGER_ALL_ACCESS );
	if (!hSCM)
	    return result;

	// Try to open the service
	SC_HANDLE hService = 0;
    QString csName = serviceName();
	hService = pOpenService( hSCM, (TCHAR*)csName.ucs2(), DELETE );

	if ( hService ) {
	    if ( pDeleteService(hService) )
		result = TRUE;
	    pCloseServiceHandle(hService);
	}

	pCloseServiceHandle (hSCM);
    }
    if ( result )
	reportEvent( "Service uninstalled" );
    else {
	reportEvent( "Uninstalling the service failed", Error );
    }

    return result;
}

bool QtService::isInstalled() const
{
    if (!sysd->available()) {
	QSettings settings;
	settings.setPath("Microsoft", "Windows");
	QString regpath = settings.readEntry("/CurrentVersion/RunServices/" + serviceName(), "Not Installed");
	if ( regpath == "Not Installed" )
	    regpath = settings.readEntry("/CurrentVersion/RunServicesManually/" + serviceName(), "Not Installed");
	if ( regpath == "Not Installed" )
	    return FALSE;
	sysd->filepath = regpath;
	return TRUE;
    }
    bool result = FALSE;

    // Open the Service Control Manager
    SC_HANDLE hSCM = pOpenSCManager( 0, 0, 0 );
    if (!hSCM)
	return result;

    // Try to open the service
    QString csName = serviceName();
    SC_HANDLE hService = pOpenService( hSCM, (TCHAR*)csName.ucs2(), SERVICE_QUERY_CONFIG );

    if (hService) {
	result = TRUE;
	pCloseServiceHandle(hService);
    }
    pCloseServiceHandle(hSCM);
    
    return result;
}

bool QtService::isRunning() const
{
    if (!sysd->available()) {
	isInstalled(); // setup filepath
	QFile pidfile( sysd->filepath + ".pid" );
	if (!pidfile.open(IO_ReadOnly))
	    return FALSE;
	QTextStream in(&pidfile);
	in >> sysd->procid;
	return 0 != GetProcessVersion(sysd->procid);
    }

    bool result = FALSE;
    // Open the Service Control Manager
    SC_HANDLE hSCM = pOpenSCManager( 0, 0, 0 );
    if (!hSCM)
	return result;

    // Try to open the service
    QString csName = serviceName();
    SC_HANDLE hService = pOpenService( hSCM, (TCHAR*)csName.ucs2(), SERVICE_QUERY_STATUS );

    if (hService) {
	SERVICE_STATUS info;
	int res = pQueryServiceStatus( hService, &info );
	if ( res )
	    result = info.dwCurrentState != SERVICE_STOPPED;

	pCloseServiceHandle(hService);
    }
    pCloseServiceHandle(hSCM);

    return result;
}

bool QtService::start()
{
    if (!sysd->available()) {
	if ( !initialize() ) {
	    reportEvent( "The Service failed to start", Error );
	    return FALSE;
	}
	QFile pidfile( sysd->filepath + ".pid" );
	pidfile.open(IO_WriteOnly);
	QTextStream out(&pidfile);
	DWORD pid = GetCurrentProcessId();
	out << pid;
	pidfile.close();
	if ( pRegisterServiceProcess )
	    pRegisterServiceProcess(pid, 1);

	QFile stdinfile;
	stdinfile.open(IO_ReadOnly, stdin);
	QByteArray array(10);
	int a = 0;
	int argc = 1;
	while (!stdinfile.atEnd()) {
	    char c = stdinfile.getch();
	    if ( c == ' ' ) {
		++argc;
		c = '\0';
	    }
	    array[a] = c;
	    if ( ++a >= array.size() )
		array.resize(a*2);
	}
	array.resize(a+1);
	array[a] = 0;
	stdinfile.close();
	if ( a )
	    argc++;

    char arg0[_MAX_PATH];
	qstrcpy(arg0, sysd->filepath);
	char **argv = new char*[argc+2];
	argv[0] = (char*)arg0;
	if ( argc > 1 ) {
	    int argc2 = 0;
	    argv[argc2+1] = array.data();	    
	    for ( int i = 0; i < array.size(); ++i ) {
		if ( 0 == array[i] && argc2+1 < argc-1 ) {
		    ++argc2;
		    argv[argc2+1] = array.data()+i+1;
		}
	    }
	    argv[argc2+2] = 0;
	}

    oldEventFilter = qt_set_win_event_filter(QtServiceSysPrivate::QtServiceEventFilter);
	int res = run( argc, argv );

	delete []argv;
	QFile::remove( sysd->filepath + ".pid" );
	return res ? FALSE : TRUE;
    }

    if ( !isInstalled() )
	return FALSE;

    const length = serviceName().length();
    QString csName = serviceName();

    SERVICE_TABLE_ENTRY st [2];
    st[0].lpServiceName = (TCHAR*)csName.ucs2();
    st[0].lpServiceProc = QtServiceSysPrivate::serviceMain;
    st[1].lpServiceName = 0;
    st[1].lpServiceProc = 0;

    bool res = pStartServiceCtrlDispatcher(st);
    if ( !res )
	reportEvent( "The Service failed to start", Error );
    return res;
}

int QtService::exec( int argc, char **argv )
{
#ifndef _DEBUG
    if ( !isInstalled() ) {
	if (!initialize())
	    return -1;
	return run(argc, argv);
    }
#else
    initialize();
	return run(argc, argv);
#endif
    if ( sysd->available() ) {
	SC_HANDLE hSCM = pOpenSCManager( 0, 0, SC_MANAGER_CONNECT );
	if ( !hSCM )
	    return GetLastError();

    QString csName = serviceName();
	SC_HANDLE hService = pOpenService( hSCM, (TCHAR*)csName.ucs2(), SERVICE_START );
	if ( !hService )
	    return GetLastError();

	// Not much point blowing up argv to wide characters - they end up being ANSI anyway.
	bool res = pStartService( hService, argc, (const char**)argv );
	if ( !res ) {
	    reportEvent( "The Service failed to execute", Error );
	    return GetLastError();
	} else {
	    return 0;
	}
    } else if ( !isRunning() ) {
	QProcess process( sysd->filepath );
	bool res = FALSE;
	QString stdIn;
	for ( int a = 0; a < argc; ++a ) {
	    stdIn += argv[a];
	    if ( a < argc-1 )
		stdIn += " ";
	}
	res = process.launch(stdIn);
	if ( !res )
	    reportEvent( "The Service failed to execute", Error );
	return res ? 0 : -1;
    }
    return -1;
}

bool QtService::terminate()
{
    if ( !isRunning() )
	return TRUE;
    if (!sysd->available()) {
	if (!sysd->procid)
	    return TRUE;
	if (sysd->findWindow())
	    PostMessage( sysd->messages, sysd->WM_QTCOMMAND, SERVICE_CONTROL_STOP, 0 );
	bool stopped = FALSE;
	int i = 0;
	while(!stopped && i < 10) {
	    Sleep(500);
	    stopped = GetProcessVersion(sysd->procid) == 0;
	}
	return stopped;
    }

    SC_HANDLE hSCM = pOpenSCManager( 0, 0, SC_MANAGER_CONNECT );
    if ( !hSCM )
	return FALSE;

    QString csName = serviceName();
    SC_HANDLE hService = pOpenService( hSCM, (TCHAR*)csName.ucs2(), SERVICE_STOP );
    if ( !hService )
	return FALSE;

    SERVICE_STATUS status;
    pControlService( hService, SERVICE_CONTROL_STOP, &status );

    bool stopped = FALSE;
    int i = 0;
    while(!stopped && i < 10) {
	Sleep(200);
	pQueryServiceStatus(hService, &status);
	stopped = status.dwCurrentState = SERVICE_STOPPED;
    }

    pCloseServiceHandle(hService);
    pCloseServiceHandle(hSCM);

    return stopped;
}

void QtService::requestPause()
{
    if (!isRunning())
	return;

    if (!sysd->available()) {
	if (sysd->findWindow())
	    PostMessage(sysd->messages, sysd->WM_QTCOMMAND, SERVICE_CONTROL_PAUSE, 0);
    } else {
	SC_HANDLE hSCM = pOpenSCManager( 0, 0, SC_MANAGER_CONNECT );
	if ( !hSCM )
	    return;

    QString csName = serviceName();
	SC_HANDLE hService = pOpenService( hSCM, (TCHAR*)csName.ucs2(), SERVICE_PAUSE_CONTINUE );
	if ( !hService )
	    return;

	SERVICE_STATUS status;
	BOOL res = pControlService( hService, SERVICE_CONTROL_PAUSE, &status );

	pCloseServiceHandle(hService);
	pCloseServiceHandle(hSCM);
    }
}

void QtService::requestResume()
{
    if (!isRunning())
	return;

    if (!sysd->available()) {
	if (sysd->findWindow())
	    PostMessage(sysd->messages, sysd->WM_QTCOMMAND, SERVICE_CONTROL_CONTINUE, 0);
    } else {
	SC_HANDLE hSCM = pOpenSCManager( 0, 0, SC_MANAGER_CONNECT );
	if ( !hSCM )
	    return;

    QString csName = serviceName();
	SC_HANDLE hService = pOpenService( hSCM, (TCHAR*)csName.ucs2(), SERVICE_PAUSE_CONTINUE );
	if ( !hService )
	    return;

	SERVICE_STATUS status;
	BOOL res = pControlService( hService, SERVICE_CONTROL_CONTINUE, &status );

	pCloseServiceHandle(hService);
	pCloseServiceHandle(hSCM);
    }
}

void QtService::sendCommand(int code)
{
    if ( code < 0 || code > 127 )
	return;

    if (!isRunning())
	return;

    if (!sysd->available()) {
	if (sysd->findWindow())
	    PostMessage(sysd->messages, sysd->WM_QTCOMMAND, 128 + code, 0);
    } else {
	SC_HANDLE hSCM = pOpenSCManager( 0, 0, SC_MANAGER_CONNECT );
	if ( !hSCM )
	    return;

    QString csName = serviceName();
	SC_HANDLE hService = pOpenService( hSCM, (TCHAR*)csName.ucs2(), SERVICE_USER_DEFINED_CONTROL );
	if ( !hService )
	    return;

	SERVICE_STATUS status;
	BOOL res = pControlService( hService, 128 + code, &status );

	pCloseServiceHandle(hService);
	pCloseServiceHandle(hSCM);
    }
}

void QtService::reportEvent( const QString &message, EventType type, int ID, uint category, const QByteArray &data )
{
    if (!sysd->available()) {
	QFile logfile( sysd->filepath + ".log" );
	logfile.open(IO_WriteOnly|IO_Append|IO_Translate);
	QTextStream out(&logfile);
	QDateTime cur = QDateTime::currentDateTime();
	QString typestr;
	switch(type) {
	case Error:
	    typestr = "Error";
	    break;
	case Warning:
	    typestr = "Warning";
	    break;
	case Information:
	    typestr = "Information";
	    break;
	default:
	    typestr = "Success";
	    break;
	}
	out << cur.toString(ISODate) << endl;
	out << "\tEvent    : " << typestr << endl;
	out << "\tMessage  : " << message << endl;
	out << "\tID       : " << ID << endl;
	out << "\tCategory : " << category << endl;
	if ( data.size() )
	    out << "Data:" << data.data() << endl;
	out << endl << endl;
	return;
    }
    WORD wType;

    switch ( type ) {
    case Error:
	wType = EVENTLOG_ERROR_TYPE;
	break;

    case Warning:
	wType = EVENTLOG_WARNING_TYPE;
	break;

    case Information:
	wType = EVENTLOG_INFORMATION_TYPE;

    default:
	wType = EVENTLOG_SUCCESS;
	break;
    }

    QString csName = serviceName();
    HANDLE h = pRegisterEventSource( 0, (TCHAR*)csName.ucs2() );
    if ( !h )
	return;
    const TCHAR *msg = (TCHAR*)message.ucs2();
    char *bindata = data.size() ? data.data() : 0;
    pReportEvent( h, wType, category, ID, 0, 1, data.size(), (const TCHAR**)&msg, bindata );

    pDeregisterEventSource( h );
}
