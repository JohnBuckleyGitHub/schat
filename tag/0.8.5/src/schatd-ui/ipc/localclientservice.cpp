/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore>
#include <QtNetwork>

#include "localclientservice.h"
#include "protocol.h"

/*!
 * \brief Конструктор класса LocalClientService.
 */
LocalClientService::LocalClientService(QObject *parent)
  : QObject(parent)
{
  m_key = QCryptographicHash::hash(qApp->applicationDirPath().toUtf8(), QCryptographicHash::Md5).toHex();
  m_reconnectTimer.setInterval(600);
  m_nextBlockSize = 0;
  m_socket = new QLocalSocket(this);
  m_stream.setVersion(StreamVersion);
  m_stream.setDevice(m_socket);
  connect(m_socket, SIGNAL(connected()), SLOT(connected()));
  connect(m_socket, SIGNAL(error(QLocalSocket::LocalSocketError)), SLOT(disconnected()));
  connect(m_socket, SIGNAL(disconnected()), SLOT(disconnected()));
  connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
  connect(&m_reconnectTimer, SIGNAL(timeout()), SLOT(connectToServer()));
}


void LocalClientService::connectToServer()
{
  m_reconnectTimer.stop();
  m_nextBlockSize = 0;
  m_socket->abort();
  m_socket->connectToServer(m_key);
}


void LocalClientService::exit()
{
  if (send(666)) {
    m_socket->waitForBytesWritten(2000);
    m_socket->disconnectFromServer();
  }
}


void LocalClientService::leave()
{
  m_socket->disconnectFromServer();
}


void LocalClientService::connected()
{
  m_reconnectTimer.stop();
  emit notify(Start);
}


void LocalClientService::disconnected()
{
  if (!m_reconnectTimer.isActive())
    m_reconnectTimer.start();

  emit notify(Stop);
}


void LocalClientService::readyRead()
{
  forever {
    if (!m_nextBlockSize) {
      if (m_socket->bytesAvailable() < (int) sizeof(quint16))
        break;

      m_stream >> m_nextBlockSize;
    }

    if (m_socket->bytesAvailable() < m_nextBlockSize)
      break;

    m_stream >> m_opcode;

    switch (m_opcode) {
      case 666:
        m_socket->disconnectFromServer();
        return;

      default:
        unknownOpcode();
        break;
    }
  }
}


bool LocalClientService::send(quint16 opcode)
{
  if (m_socket->state() == QLocalSocket::ConnectedState) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << opcode;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
}


void LocalClientService::unknownOpcode()
{
  QByteArray block = m_socket->read(m_nextBlockSize - (int) sizeof(quint16));
  m_nextBlockSize = 0;
}
