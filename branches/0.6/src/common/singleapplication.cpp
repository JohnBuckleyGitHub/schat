/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
 *
 * class SingleApplication Copyright © 2008, Benjamin C. Meyer <ben@meyerhome.net>
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

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QLocalServer>
#include <QLocalSocket>
#include <QTextStream>

#ifndef Q_OS_WIN
#include <unistd.h>
#endif

/*!
 * Конструктор класса SingleApplication.
 */
SingleApplication::SingleApplication(int &argc, char **argv)
  : QApplication(argc, argv), m_localServer(0)
{
}


/*!
 * Отправка сообщения другому экземпляру, используется для
 * передачи командной строки.
 *
 * \return \a true в случае успешной отправки, что свидетельствует
 * о наличии другого экземпляра приложения.
 */
bool SingleApplication::sendMessage(const QString &message)
{
  #ifdef SCHAT_DEVEL_MODE
  return false;
  #endif

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


/*!
 * Запуск локального сервера.
 *
 * \return \a true в случае успешного запуска сервера.
 */
bool SingleApplication::startSingleServer()
{
  #ifdef SCHAT_DEVEL_MODE
  return true;
  #endif

  if (m_localServer)
    return false;

  m_localServer = new QLocalServer(this);
  connect(m_localServer, SIGNAL(newConnection()), SLOT(newConnection()));

  bool success = false;

  if (!m_localServer->listen(serverName())) {
    if (m_localServer->serverError() == QAbstractSocket::AddressInUseError) {
      // cleanup from a segfaulted server
      #ifdef Q_OS_UNIX
      QString fullServerName = QDir::tempPath() + QLatin1String("/") + serverName();
      if (QFile::exists(fullServerName))
        QFile::remove(fullServerName);
      #endif

      if (m_localServer->listen(serverName()))
        success = true;
    }
  }
  else
    success = true;

  #ifdef Q_OS_UNIX
  if (success) {
    QFile file(m_localServer->fullServerName());
    file.setPermissions(QFile::ReadUser | QFile::WriteUser);
  }
  #endif

  if (!success) {
    delete m_localServer;
    m_localServer = 0;
  }

  return success;
}


bool SingleApplication::isRunning() const
{
  #ifdef SCHAT_DEVEL_MODE
  return true;
  #endif

  return (0 != m_localServer);
}


void SingleApplication::newConnection()
{
  QLocalSocket *socket = m_localServer->nextPendingConnection();
  if (!socket)
    return;

  socket->waitForReadyRead(300);
  QTextStream stream(socket);
  QString message = stream.readAll();
  emit messageRecieved(message);
  delete socket;
}


QString SingleApplication::serverName() const
{
  static QString s_serverName;

  if (!s_serverName.isEmpty())
    return s_serverName;

  QString serverName = QCoreApplication::applicationName();

  #ifdef Q_WS_QWS
  serverName += QLatin1String("_qws");
  #endif

  #ifndef Q_OS_WIN
  serverName += QString(QLatin1String("_%1_%2")).arg(getuid()).arg(getgid());
  #else
  serverName += '_' + QDir::home().dirName();
  #endif

  s_serverName = QCryptographicHash::hash(serverName.toUtf8(), QCryptographicHash::Md5).toHex();
  return s_serverName;
}
