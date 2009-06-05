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
#include "qtunixsocket.h"
#include "qtunixserversocket.h"
#include <qapplication.h>
#include <pwd.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include <sys/stat.h>

class QtServiceSysPrivate : public QtUnixServerSocket
{
    Q_OBJECT
public:
    QtServiceSysPrivate();
    ~QtServiceSysPrivate();

    QString socketPath() const;
    QString encodeName(const QString &name, bool allowUpper = FALSE) const;
    void newConnection(int socket);
    bool sendCmd(const QString &cmd) const;

    QString login;

    int fdstdin, fdstdout, fdstderr;

    char *ident;

private slots:
    void slotReady();
    void slotClosed();

private:
    QString getCommand(const QSocket *socket);
    QMap<const QSocket *, QString> cache;
};

QtServiceSysPrivate::QtServiceSysPrivate()
    : ident(0)
{
    uid_t uid = getuid();
    passwd *pw = getpwuid(uid);
    if (pw)
	login = QString(pw->pw_name);
}

QtServiceSysPrivate::~QtServiceSysPrivate()
{
    if (ident)
	delete[] ident;
}

void QtServiceSysPrivate::newConnection(int socket)
{
    QSocket *s = new QSocket(this);
    s->setSocket(socket);
    connect(s, SIGNAL(readyRead()), this, SLOT(slotReady()));
    connect(s, SIGNAL(connectionClosed()), this, SLOT(slotClosed()));
}

bool QtServiceSysPrivate::sendCmd(const QString &cmd) const
{
    QtUnixSocket sock;
    if (sock.connectTo(socketPath())) {
	sock.writeBlock(QString(cmd+"\r\n").latin1(), cmd.length()+2);
	sock.flush();
	sock.close();
	return true;
    }
    return false;
}

void QtServiceSysPrivate::slotReady()
{
    QSocket *s = (QSocket *)sender();
    cache[s] += QString(s->readAll());
    QString cmd = getCommand(s);
    while (!cmd.isEmpty()) {
	if (cmd == "terminate")
	    qService->stop();
	else if (cmd == "pause")
	    qService->pause();
	else if (cmd == "resume")
	    qService->resume();
	else if (cmd.length() > 4 && cmd.left(4) == "num:") {
	    cmd = cmd.mid(4);
	    qService->user(cmd.toInt());
	}
	cmd = getCommand(s);
    }
}

void QtServiceSysPrivate::slotClosed()
{
    QSocket *s = (QSocket *)sender();
    s->deleteLater();
}

QString QtServiceSysPrivate::getCommand(const QSocket *socket)
{
    int pos = cache[socket].find("\r\n");
    if (pos >= 0) {
	QString ret = cache[socket].left(pos);
	cache[socket].remove(0, pos+2);
	return ret;
    }
    return "";
}

QString QtServiceSysPrivate::encodeName(const QString &name, bool allowUpper) const
{
    QString n = name.lower();
    QString legal = "abcdefghijklmnopqrstuvwxyz1234567890";
    if (allowUpper)
	legal += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unsigned int pos = 0;
    while (pos < n.length()) {
	if (legal.find(n[pos]) == -1)
	    n.remove(pos, 1);
	else
	    ++pos;
    }
    return n;
}

QString QtServiceSysPrivate::socketPath() const
{
    QString sn = encodeName(qService->serviceName());
    return "/var/tmp/"+sn+"."+login;
}

#include "qtservice_unix.moc"

void QtService::sysInit()
{
    sysd = new QtServiceSysPrivate;
}

void QtService::sysCleanup()
{
    delete sysd;
    sysd = 0;
}

bool QtService::install()
{
    return true;
}

bool QtService::uninstall()
{
    return true;
}

bool QtService::isInstalled() const
{
    return false;
}

bool QtService::isRunning() const
{
    QtUnixSocket sock;
    if (sock.connectTo(sysd->socketPath()))
	return true;
    return false;
}

bool QtService::start()
{
    if (sysd->sendCmd("alive")) {
	return false;
    }
    if (initialize()) {
	sysd->setPath(sysd->socketPath());
	pid_t t = fork();
	if (t < 0)
	    return false;
	if (t != 0)
	    exit(0);

	// Create session & set pgid
	setsid();

        // Make sure that the child process cannot
        // acquire a controlling terminal
        ::signal(SIGHUP, SIG_IGN);
        t = fork();
        if (t < 0)
            return false;
        if (t != 0)
            exit(0);

	// Close stdin, stdout and stderr
	for (int i = 0; i <= 2; ++i)
	    ::close(i);

	// Reopen stdin, stdout and stderr
	sysd->fdstdin = ::open("/dev/null", O_RDWR);
	sysd->fdstdout = ::dup(sysd->fdstdin);
	sysd->fdstderr = ::dup(sysd->fdstdin);

	// Restrict permissions on files that are created by the service
	umask(027);

	// Change the directory to /
	chdir("/");

	run(0, NULL);

	sysd->close();

	return true;
    }
    return false;
}

int QtService::exec(int argc, char **argv)
{
    if (!isInstalled()) {
	if (!initialize())
	    return -1;
	return run(argc, argv);
    }
    return -1;
}

bool QtService::terminate()
{
    return sysd->sendCmd("terminate");
}

void QtService::requestPause()
{
    sysd->sendCmd("pause");
}

void QtService::requestResume()
{
    sysd->sendCmd("resume");
}

void QtService::sendCommand(int code)
{
    sysd->sendCmd("num:"+QString::number(code));
}

void QtService::reportEvent(const QString &message, EventType type,
			    int, uint, const QByteArray &)
{
    int st;
    switch(type) {
    case Error:
	st = LOG_ERR;
	break;
    case Warning:
	st = LOG_WARNING;
	break;
    default:
	st = LOG_INFO;
    }
    if (!sysd->ident) {
	QString tmp = sysd->encodeName(serviceName(), TRUE);
	uint len = tmp.local8Bit().length();
	sysd->ident = new char[len+1];
	sysd->ident[len] = '\0';
	::memcpy(sysd->ident, tmp.local8Bit().data(), len);
    }
    openlog(sysd->ident, LOG_PID, LOG_DAEMON);
    syslog(st, message.local8Bit());
    closelog();
}
