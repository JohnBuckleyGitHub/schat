/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include <QCoreApplication>
#include <QDataStream>

#include "protocol.h"
#include "ipc/localservice.h"

/*!
 * Конструктор класса LocalService.
 */
LocalService::LocalService(QLocalSocket *socket, QObject *parent)
  : QObject(parent), m_socket(socket)
{
  if (m_socket) {
    m_socket->setParent(this);
    connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(m_socket, SIGNAL(disconnected()), SLOT(deleteLater()));
    m_nextBlockSize = 0;
    m_stream.setDevice(m_socket);
    m_stream.setVersion(StreamVersion);
  }
  else
    deleteLater();
}


LocalService::~LocalService()
{
  if (m_socket && m_socket->state() == QLocalSocket::ConnectedState && send(666)) {
//    m_socket->waitForBytesWritten(500);
    m_socket->close();
  }
}


void LocalService::readyRead()
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
        QCoreApplication::quit();
        return;

      default:
        unknownOpcode();
        break;
    }
  }
}


bool LocalService::send(quint16 opcode)
{
  if (m_socket->state() == QLocalSocket::ConnectedState) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << quint16(0) << opcode;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
}


void LocalService::unknownOpcode()
{
  QByteArray block = m_socket->read(m_nextBlockSize - (int) sizeof(quint16));
  m_nextBlockSize = 0;
}
