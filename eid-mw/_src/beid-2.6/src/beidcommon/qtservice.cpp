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
#include <stdio.h>

QtService *qService = 0;

class QtServicePrivate
{
public:
    QString servicename;
    QString servicedesc;
    QtService::StartupType starttype;
};

/*!
    \class QtService qtservice.h
    \brief The QtService class provides an API from implementing Windows
    services and Unix daemons.

    A Windows service or Unix daemon (a "service"), is a program that
    runs regardless of whether a user is logged in or not. Services
    are usually non-interactive console applications, but can also
    provide a user interface.

    A service is installed in a system's service database with
    install(). The system will start the service on startup, depending
    on the \c StartupType specified. A service can also be started
    explicitly with exec().

    On Windows 95, 98 and ME the service is registered in the Windows
    Registry's \c RunServices entry.

    On Windows NT based systems, such as Windows NT 4, 2000, and XP,
    the implementation uses the NT Service Control Manager, and the
    application can be controlled through the system administration
    tools. Services are usually launched using the system account,
    which requires that all DLLs that the service executable depends
    on (i.e. Qt) are located in the same directory as the service, or
    in a system path.

    On Unix the service is implemented as a daemon.

    When a service is started, it calls initialize(), and then the
    run() implementation which usually creates a QApplication object
    and enters the event loop. The stop() implementation must make the
    application leave the event loop, so that the run() implementation
    can return; usually this is done by calling \c{qApp->quit()}.

    On Windows NT based systems the service control manager can send
    commands to the service to pause(), resume(), or stop() the
    service, as well as service specific user() commands. This can be
    achieved on other systems by running the executable itself with
    suitable command line arguments.

    A service can report events to the system's event log with
    reportEvent(). The status of the service can be queried with
    isInstalled() and isRunning().

    A running service can be stopped by the service control manager,
    or by calling terminate(). If the service is no longer needed it
    can be uninstalled from the system's service database with
    uninstall().

    The implementation of a service application's main() entry point
    function usually creates a QtService object, calls
    parseArguments(), and returns the result of that call.

    When a service object is destroyed the service is not \link stop()
    stopped\endlink and is not \link uninstall() uninstalled\endlink.

    \warning On Windows, different versions have different limitations
    as to what a service is allowed to do. For example, on Windows XP
    Home Edition the Local System Account does not have the privilege
    to "Logon as a service". If you have problems with a service
    consult the documentation provided by Microsoft, i.e. in MSDN.
*/

/*!
    \enum QtService::EventType

    This enum describes the different types of events reported by a
    service to the system log.

    \value Success An operation has succeeded, e.g. the service has
    been installed.

    \value Error An operation failed, e.g. the service failed to
    start.

    \value Warning An operation caused a warning that might require
    user interaction.

    \value Information Any type of usually non-critical information.
*/

/*!
    \enum QtService::StartupType

    This enum describes when a service should be started.

    \value Auto The service is started during system startup.
    \value Manual The service is started when a process starts it.
*/

/*!
    Constructs a QtService object called \a name and with the optional
    description \a desc. \a startup specifies when the service should
    be started. The default is \c Auto.

    There can only be one QtService object in a process.

    The service is not installed or started. The \a name must not
    contain any backslashes, cannot be longer than 255 characters, and
    must be unique in the system's service database.

    \sa install(), exec()
*/
QtService::QtService( const QString &name, const QString &desc, StartupType startup )
{
    if ( qService )
	qFatal( "Only one instance of QtService can exist." );

    QString nm( name );
    if ( nm.length() > 255 ) {
	qWarning( "QtService: 'name' is longer than 255 characters." );
	nm.truncate( 255 );
    }
    if ( nm.contains( '\\' ) ) {
	qWarning( "QtService: 'name' contains backslashes '\\'." );
	nm.replace( (QChar)'\\', (QChar)'\0' );
    }

    d = new QtServicePrivate();
    d->servicename = nm;
    d->servicedesc = desc;
    d->starttype = startup;
    qService = this;

    sysInit();
}

/*!
    Destroys this service object. This does not stop
    or uninstall the service.

    \sa uninstall()
*/
QtService::~QtService()
{
    sysCleanup();

    if ( qService == this )
	    qService = 0;
    delete d;
}

/*!
    Parses \a argc command line arguments in \a argv and returns an error code,
    or zero if no error occurred.

    The following arguments are recognized:

    \table
    \header \i Short \i Long \i Explanation
    \row \i -i \i -install \i Calls install() to install the service
    \row \i -u \i -uninstall \i Calls uninstall() to uninstall the service
    \row \i -e \i -exec \i Calls exec() to execute the service application
    \row \i -t \i -terminate \i Calls terminate() to stop the service application
    \row \i -p \i -pause \i Call requestPause() to pause the service application
    \row \i -r \i -resume
	 \i Call requestResume() to resume a paused service application
    \row \i -c \e{cmd} \i -command \e{cmd}
	 \i Send the user defined command code \e{cmd} to the service application
    \row \i -v \i -version \i Displays version and status information
    \endtable

    If no argument is provided the service calls start() to run the service and
    listen to commands from the service control manager.
*/
int QtService::parseArguments( int argc, char **argv )
{
    if ( argc > 1 ) {
	QString a( argv[1] );
	if ( a == "-i" || a == "-install" ) {
	    if ( !isInstalled() ) {
		if ( !install() ) {
		    fprintf(stderr, "The service %s could not be installed\n", serviceName().latin1());
		    return -1;
		}
	    } else {
		fprintf(stderr, "The service %s is already installed\n", serviceName().latin1());
	    }
	} else if ( a == "-u" || a == "-uninstall" ) {
	    if ( isInstalled() ) {
		if ( !uninstall() ) {
		    fprintf(stderr, "The service %s could not be uninstalled\n", serviceName().latin1());
		    qSystemWarning( "Reason for failure:" );
		    return -1;
		}
	    } else {
		fprintf(stderr, "The service %s is not installed\n", serviceName().latin1());
	    }
	} else if ( a == "-v" || a == "-version" ) {
	    printf("The service\n"
		   "\t%s\n\t%s\n\n", serviceName().latin1(), argv[0]);
	    printf("is %s", (isInstalled() ? "installed" : "not installed"));
	    printf(" and %s\n\n", (isRunning() ? "running" : "not running"));
	} else if ( a == "-e" || a == "-exec" ) {
	    int ec = exec( argc - 2, argv + 2 );
	    if ( ec )
		qSystemWarning( "The service could not be started.", ec );
	    return ec;
	} else if ( a == "-t" || a == "-terminate" ) {
	    if ( !terminate() )
		qSystemWarning( "The service could not be stopped." );
	} else if ( a == "-p" || a == "-pause" ) {
	    requestPause();
	} else if ( a == "-r" || a == "-resume" ) {
	    requestResume();
	} else if ( a == "-c" || a == "-command" ) {
	    int code = 0;
	    if ( argc > 2 ) {
		QString codestr(argv[2]);
		code = codestr.toInt();
	    }
	    sendCommand( code );
	} else {
	    printf("<service> -[i|u|e|s|v]\n\n"
		   "\t-i(nstall)\t: Install the service\n"
		   "\t-u(ninstall)\t: Uninstall the service\n"
		   "\t-e(xec)\t\t: Execute the service\n"
		   "\t\t\t  If the service is not installed, run it as a regular program\n"
		   "\t-t(erminate)\t: Stop the service\n"
		   "\t-v(ersion)\t: Print version and status information\n");
	}
    } else {
	if ( !start() ) {
	    fprintf(stderr, "The service %s could not start\n", serviceName().latin1());
	    return -4;
	}
    }

    return 0;
}

/*!
    \fn bool QtService::install()

    Installs the service in the system's service control manager and
    returns true if successful; otherwise returns false.

    The service reports the result of the installation to the system's
    event log.

    The default implementation of parseArguments() calls this function
    when the command line option \c -i (or \c -install) is used.

    \warning Due to the different implementations of how services
    (daemons) are installed on various UNIX-like systems, this function
    is not implemented on such systems.

    \sa uninstall()
*/

/*!
    \fn bool QtService::uninstall()

    Uninstalls the service from the system's default service control
    manager and returns true if successful; otherwise returns false.

    The service reports the result of the uninstallation to the
    system's event log.

    The default implementation of parseArguments() calls this function
    when the command line option \c -u (or \c -uninstall) is used.

    \warning Due to the different implementations of how services
    (daemons) are installed on various UNIX-like systems, this function
    is not implemented on such systems.

    \sa install(), isInstalled()
*/

/*!
    \fn bool QtService::isInstalled() const

    Returns true if the service is installed in the system's default service
    control manager; otherwise returns false.

    \warning This function always returns false on UNIX-like systems.

    \sa install()
*/

/*!
    \fn bool QtService::isRunning() const

    Returns true if the service is running; otherwise returns false.

    A service must be installed before it can be run, except on
    UNIX-like systems; see install().

    Note that isRunning() returns false if the program runs
    stand-alone, so you can modify your application's behavior
    depending on the context it is running in:
    \code
    int MyService::run( int argc, char **argv )
    {
        QApplication app( argc, argv );

        QWidget *gui = new ServiceGui( ... );
	if ( !isRunning() ) // running stand alone -> quit when GUI is closed
	    app.setMainWidget( gui );

        gui->show();
	return app.exec();
    }
    \endcode

    \sa isInstalled(), exec(), start(), stop()
*/

/*!
    Returns the service's startup type.

    \sa StartupType
*/
QtService::StartupType QtService::startupType() const
{
    return d->starttype;
}

/*!
    \fn bool QtService::start()

    Starts the service, and returns true if successful; otherwise
    returns false.

    A service must be installed before it can be started, except on
    UNIX-like systems; see install().

    The service reports an error \c EventType to the system event log
    if starting the service fails.

    The default implementation of parseArguments() calls this function
    if no command line options are used.

    \sa install(), exec(), stop(), isRunning()
*/

/*!
    \fn int QtService::run( int argc, char **argv )

    This pure virtual function must be implemented in derived classes
    in order to perform the service's work. Usually you create the
    QtService subclass instance in main() and return the value of
    a call to parseArguments(). In the subclass's run() function you
    create the QApplication object passing \a argc and \a argv,
    initialize the application, and enter the event loop (e.g. by
    calling \c{return app.event();} in this function.

    \sa start(), exec(), terminate(), stop()
*/

/*!
    Stops the service. The default implementations calls quit() if
    there is a QApplication object, otherwise it does nothing.
    Reimplement this function if you want to perform additional
    cleanups before shutting down.

    \sa run(), terminate()
*/
void QtService::stop()
{
    if ( qApp )
	qApp->quit();
}

/*!
    \fn int QtService::exec( int argc, char **argv )

    Tries to start the service, passing \a argc and \a argv to the
    run() implementation.

    If the service is installed it is started as a separate process,
    and exec() returns immediately with the return value 0 if the
    service could be started. Otherwise a non-zero error code is
    returned and an error event is reported to the system's event log.

    If the service is not installed the run() implementation is called
    directly, and the result is returned.

    The default implementation of parseArguments() calls this function
    when the command line option \c -e (or \c -exec) is used.

    \sa install(), start(), run()
*/

/*!
    \fn bool QtService::terminate()

    Asks the service control manager to stop the service if the
    service is running, otherwise does nothing. Returns true if the
    service could be stopped (or was not running); otherwise returns
    false.

    The default implementation of parseArguments() calls this function
    when the command line option \c -t (or \c -terminate) is used.

    \sa exec(), isRunning()
*/

/*!
    Reimplement this function to perform service dependent initialization
    routines, and return true if successful; otherwise return false.

    The default implementation does nothing and returns true.

    \sa run()
*/
bool QtService::initialize()
{
    return TRUE;
}

/*!
    Reimplement this function to pause the service's execution (e.g.
    stop a polling timer, or to ignore socket notifiers).

    The default implementation does nothing.

    \sa resume(), requestPause()
*/
void QtService::pause()
{
}

/*!
    \fn void QtService::requestPause()

    Requests the running service to pause. The service will
    call the pause() implementation.

    This function does nothing if the service is not running.

    \sa requestResume()
*/

/*!
    Reimplement this function to continue the service after
    a call to pause().

    The default implementation does nothing.

    \sa pause(), requestResume()
*/
void QtService::resume()
{
}

/*!
    \fn void QtService::requestResume()

    Requests a paused service to continue. The service will
    call the resume() implementation.

    This function does nothing if the service is not running.

    \sa requestPause()
*/

/*!
    Reimplement this function to process the user command
    \a code.

    The default implementation does nothing.

    \sa sendCommand()
*/
void QtService::user( int code )
{
    Q_UNUSED(code)
}

/*!
    \fn void QtService::sendCommand(int code)

    Sends the user command \a code to the service. The service
    will call the user() implementation.

    This function does nothing if the service is not running.
*/

/*!
    Returns the name of the service.

    \sa serviceDescription()
*/
QString QtService::serviceName() const
{
    return d->servicename;
}

/*!
    Returns the description of the service.

    \sa serviceName()
*/
QString QtService::serviceDescription() const
{
    return d->servicedesc;
}

/*!
    \fn void QtService::reportEvent( const QString &message, EventType type, int ID, uint category, const QByteArray &data )

    Report an event of type \a type with text \a message to the local
    system event log. The event identifier \a ID and the event
    category \a category are user defined values. \a data can contain
    arbitrary binary data.

    Message strings for \a ID and \a category must be provided by a
    message file, which must be registered in the system registry.
    Refer to the MSDN for more information about how to do this on
    Windows.
*/
