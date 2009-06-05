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
#include "qtunixserversocket.h"
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#ifndef SUN_LEN
#define SUN_LEN(ptr) ((size_t)(((struct sockaddr_un *) 0)->sun_path) \
                      +strlen ((ptr)->sun_path))
#endif

QtUnixServerSocket::QtUnixServerSocket(const QString &path,
                                       QObject *parent,
                                       const char *name)
    : QServerSocket(parent, name)
{
    setPath(path);
}

QtUnixServerSocket::QtUnixServerSocket(QObject *parent,
                                       const char *name)
    : QServerSocket(parent, name)
{
}

void QtUnixServerSocket::setPath(const QString &path)
{
    path_ = QString::null;

    int sock = ::socket(PF_UNIX, SOCK_STREAM, 0);
    if (sock != -1) {
	struct sockaddr_un addr;
	::memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	::unlink(path.latin1()); // ### This might need to be changed
	unsigned int pathlen = strlen(path.latin1());
	if (pathlen > sizeof(addr.sun_path)) pathlen = sizeof(addr.sun_path);
	::memcpy(addr.sun_path, path.latin1(), pathlen);
	if ((::bind(sock, (struct sockaddr *)&addr, SUN_LEN(&addr)) != -1) &&
	    (::listen(sock, 5) != -1)) {
	    setSocket(sock);
            path_ = path;
        }
    }
}

void QtUnixServerSocket::close()
{
    socketDevice()->close();
    if (!path_.isEmpty()) {
        ::unlink(path_.latin1());
        path_ = QString::null;
    }
}
