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
#ifndef QTSERVICE_H
#define QTSERVICE_H

#include <qstring.h>
#include <qnamespace.h>

class QtServicePrivate;
class QtServiceSysPrivate;

class QtService : public Qt
{
public:
    enum EventType
    {
	Success = 0, Error, Warning, Information
    };
    enum StartupType
    {
	Auto = 0, Manual
    };

    QtService( const QString &name, const QString &desc = QString::null, StartupType startup = Auto );
    virtual ~QtService();

    int parseArguments( int argc, char **argv );

    virtual bool isInstalled() const;
    virtual bool isRunning() const;
    StartupType startupType() const;

    virtual bool install();
    virtual bool uninstall();
    virtual void reportEvent( const QString &, EventType type = Success, int ID = 0, uint category = 0, const QByteArray &data = QByteArray() );
    virtual void powerEvent( unsigned long, void *,  void *) {};

    virtual int exec( int argc, char **argv );
    virtual bool terminate();
    virtual void requestPause();
    virtual void requestResume();
    virtual void sendCommand(int code);

    QString serviceName() const;
    QString serviceDescription() const;

protected:
    virtual bool initialize();
    virtual int  run( int argc, char **argv ) = 0;
    virtual bool start();
    virtual void stop();
    virtual void pause();
    virtual void resume();
    virtual void user( int code );

private:
    friend class QtServiceSysPrivate;
    void sysInit();
    void sysCleanup();

    QtServicePrivate *d;
    QtServiceSysPrivate *sysd;
};

extern QtService *qService;

#endif // QTSERVICE_H
