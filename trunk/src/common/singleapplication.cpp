/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
 *
 * class SingleApplication Copyright © 2008, Benjamin C. Meyer
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "singleapplication.h"

#include <qdir.h>
#include <qlocalserver.h>
#include <qlocalsocket.h>
#include <qtextstream.h>
#include <qfile.h>

#ifndef Q_OS_WIN
#include <unistd.h>
#else
#include <windows.h>
#endif

SingleApplication::SingleApplication(int &argc, char **argv)
    : QApplication(argc, argv), m_localServer(0)
{
}


bool SingleApplication::sendMessage(const QString &message)
{
  QLocalSocket socket;
  socket.connectToServer(serverName());
  if (socket.waitForConnected(500)) {
    QTextStream stream(&socket);
    stream << message;
    stream.flush();
    if (socket.waitForBytesWritten())
      return true;
    // if the message was sent before waitForBytesWritten was called
    // it will return false
    if (socket.error() == QLocalSocket::UnknownSocketError)
      return true;
  }
  return false;
}


bool SingleApplication::startSingleServer()
{
  if (m_localServer)
    return false;

  m_localServer = new QLocalServer(this);
  connect(m_localServer, SIGNAL(newConnection()), SLOT(newConnection()));

  bool success = false;
  if (!m_localServer->listen(serverName())) {
    if (QAbstractSocket::AddressInUseError == m_localServer->serverError()) {
      // cleanup from a segfaulted server
#ifdef Q_OS_UNIX
      QString fullServerName = QDir::tempPath() + QLatin1String("/")
          + serverName();
      if (QFile::exists(fullServerName))
        QFile::remove(fullServerName);
#endif
      if (m_localServer->listen(serverName())) {
        success = true;
      }
    }
    if (!success) {
      qWarning() << "SingleApplication: Unable to listen:"
          << m_localServer->errorString();
    }
  }
  else {
    success = true;
  }

  if (success) {
    QFile file(m_localServer->fullServerName());
    if (!file.setPermissions(QFile::ReadUser | QFile::WriteUser))
      qWarning() << "SingleApplication: Unable to set permissions on:"
          << file.fileName() << file.errorString();
  }

  if (!success) {
    delete m_localServer;
    m_localServer = 0;
  }
  return success;
}


bool SingleApplication::isRunning() const
{
  return (0 != m_localServer);
}


void SingleApplication::newConnection()
{
  QLocalSocket *socket = m_localServer->nextPendingConnection();
  if (!socket)
    return;
  socket->waitForReadyRead();
  QTextStream stream(socket);
  QString message;
  stream >> message;emit
  messageRecieved(message);
  delete socket;
}


QString SingleApplication::serverName() const
{
  QString serverName = QCoreApplication::applicationName();
  Q_ASSERT(!serverName.isEmpty());
#ifdef Q_WS_QWS
  serverName += QLatin1String("_qws");
#endif
#ifndef Q_OS_WIN
  serverName += QString(QLatin1String("_%1_%2")).arg(getuid()).arg(getgid());
#else
  static QString login;
  if (login.isEmpty()) {
    QT_WA( {
          wchar_t buffer[256];
          DWORD bufferSize = sizeof(buffer) / sizeof(wchar_t) - 1;
          GetUserNameW(buffer, &bufferSize);
          login = QString::fromUtf16((ushort*)buffer);
        },
        {
          char buffer[256];
          DWORD bufferSize = sizeof(buffer) / sizeof(char) - 1;
          GetUserNameA(buffer, &bufferSize);
          login = QString::fromLocal8Bit(buffer);
        });
  }
  serverName += QString::fromAscii("_%1").arg(login);
#endif
  return serverName;
}
