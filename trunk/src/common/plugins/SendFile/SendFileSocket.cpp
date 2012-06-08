/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include <QDebug>

#include <QBasicTimer>
#include <QTimerEvent>
#include <QFile>

#include "net/SimpleID.h"
#include "SendFileSocket.h"
#include "SendFileTransaction.h"

#define DISCOVERY_TIMEOUT 8000
#define HANDSHAKE_TIMEOUT 8000
#define RECONNECT_TIMEOUT 500

namespace SendFile {


Socket::Socket(QObject *parent)
  : QTcpSocket(parent)
  , m_release(false)
  , m_role(-1)
  , m_mode(HandshakeMode)
  , m_port(0)
  , m_nextBlockSize(0)
{
  init();
  setMode(HandshakeMode);
}


Socket::Socket(const QString& host, quint16 port, const QByteArray &id, QObject *parent)
  : QTcpSocket(parent)
  , m_release(false)
  , m_role(-1)
  , m_mode(UnknownMode)
  , m_id(id)
  , m_host(host)
  , m_port(port)
  , m_nextBlockSize(0)
{
  init();
  discovery();
}


Socket::~Socket()
{
  qDebug() << "~Socket()";

  if (m_timer->isActive())
    m_timer->stop();

  delete m_timer;
}


void Socket::accept(char code)
{
  qDebug() << "Socket::accept()" << this;

  qint32 size = 1;
  QByteArray data;

  data.reserve(size + 4);
  data.append((char*)&size, 4);
  data.append(code);

  write(data);
}


void Socket::leave(bool remove)
{
  setMode(UnknownMode);

  if (state() == ConnectedState) {
    flush();
    disconnectFromHost();
    if (m_port && state() != UnconnectedState) {
      if (!waitForDisconnected(1000))
        abort();
    }
  }
  else {
    abort();
    disconnected();
  }

  if (remove)
    deleteLater();
}


/*!
 * Отказ в авторизации.
 */
void Socket::reject()
{
  if (state() != ConnectedState)
    return;

  qint32 size = 1;
  QByteArray data;

  data.reserve(size + 4);
  data.append((char*)&size, 4);
  data.append('R');

  write(data);
  leave(true);
}


void Socket::setFile(int role, QFile *file)
{
  qDebug() << "Socket::setFile" << role << file->fileName() << file->size();
  m_role = role;
  m_file = file;

  if (!role)
    write(file->readAll());
}


void Socket::timerEvent(QTimerEvent *event)
{
  if (event->timerId() != m_timer->timerId()) {
    QTcpSocket::timerEvent(event);
    return;
  }

  qDebug() << "TIMER EVENT";

  if (m_mode == DiscoveringMode || m_mode == HandshakeMode) {
    if (state() != UnconnectedState) {
      abort();
      setSocketError(QAbstractSocket::SocketTimeoutError);
      setSocketState(QAbstractSocket::UnconnectedState);
      disconnected();
    }
    else
      discovery();
  }
}


/*!
 * Обработка подключения.
 */
void Socket::connected()
{
  qDebug() << "connected()                       " << this;

# if QT_VERSION >= 0x040600
  setSocketOption(QAbstractSocket::KeepAliveOption, 1);
# endif

  if (m_mode == DiscoveringMode) {
    setMode(HandshakeMode);

    qint32 size = 22;
    QByteArray hello;

    hello.reserve(size + 4);
    hello.append((char*)&size, 4);
    hello.append('H');
    hello.append(m_id);

    write(hello);
  }
}


void Socket::disconnected()
{
  qDebug() << "disconnected()                    " << this;
  if (m_release)
    return;

  m_release = true;
  m_timer->stop();

  if (m_mode == HandshakeMode && !m_port) {
    deleteLater();
    return;
  }

  if (m_mode == DiscoveringMode || m_mode == HandshakeMode)
    m_timer->start(RECONNECT_TIMEOUT, this);
}


void Socket::discovery()
{
  qDebug() << "Socket::discovery()" << m_host << m_port;
  if (!m_port)
    return;

  m_timer->stop();
  m_release = false;

  setMode(DiscoveringMode);
  connectToHost(m_host, m_port);
}


void Socket::error(QAbstractSocket::SocketError socketError)
{
  qDebug() << "error()                           " << this;
  qDebug() << "^^^^^" << socketError << errorString();

  if (state() != ConnectedState)
    disconnected();
}


/*!
 * Слот вызывается при получении новых данных.
 * Если данных достаточно для чтения транспортного пакета происходит чтение пакета.
 *
 * \sa readTransport()
 */
void Socket::readyRead()
{
  qDebug() << "readyRead()                       " << this << bytesAvailable();

  forever {
    if (m_mode == DataMode && m_role == ReceiverRole) {
      qint64 bytes = bytesAvailable();
      m_file->write(read(bytes));
      m_file->flush();
      return;
    }

    if (!m_nextBlockSize) {
      if (bytesAvailable() < 4)
        break;

      read((char*)&m_nextBlockSize, 4);
    }

    if (bytesAvailable() < m_nextBlockSize)
      break;

    if (m_mode == HandshakeMode && m_nextBlockSize)
      readHandshake();

    if (m_nextBlockSize) {
      read(m_nextBlockSize);
      m_nextBlockSize = 0;
    }
  }
}


void Socket::init()
{
  m_timer = new QBasicTimer();

  connect(this, SIGNAL(connected()), SLOT(connected()));
  connect(this, SIGNAL(disconnected()), SLOT(disconnected()));
  connect(this, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(error(QAbstractSocket::SocketError)));
  connect(this, SIGNAL(readyRead()), SLOT(readyRead()));
}


/*!
 * Установка режима работы сокета.
 */
void Socket::setMode(Mode mode)
{
  if (mode == DiscoveringMode) {
    m_timer->start(DISCOVERY_TIMEOUT, this);
  }
  else if (mode == HandshakeMode) {
    m_timer->start(HANDSHAKE_TIMEOUT, this);
  }
  else if (mode == DataMode) {
    m_timer->stop();
  }

  m_mode = mode;
}


void Socket::readHandshake()
{
  char code;
  getChar(&code);
  m_nextBlockSize--;
  qDebug() << code;

  if (code == 'H' && m_nextBlockSize >= 21) {
    m_nextBlockSize += 21;
    m_id = read(21);
    if (SimpleID::typeOf(m_id) == SimpleID::MessageId) {
      emit handshake(m_id);
    }
  }
  else if (code == 'R') {
    leave();
    setMode(DiscoveringMode);
    m_timer->start(2000, this);
  }
  else if (code == 'A') {
    setMode(DataMode);
    accept('a');
    emit accepted();
  }
  else if (code == 'a') {
    setMode(DataMode);
    emit accepted();
  }
}

} // namespace SendFile
