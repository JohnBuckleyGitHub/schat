/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#define SCHAT_DISABLE_DEBUG

#include <QBasicTimer>
#include <QSslConfiguration>
#include <QSslKey>
#include <QTimerEvent>

#include "debugstream.h"
#include "net/PacketReader.h"
#include "net/PacketWriter.h"
#include "net/SimpleSocket.h"
#include "net/TransportReader.h"
#include "net/TransportWriter.h"

SimpleSocket::SimpleSocket(quint64 id, QObject *parent)
  : QSslSocket(parent)
  , m_authorized(false)
  , m_release(false)
  , m_serverSide(false)
  , m_sslAvailable(false)
  , m_id(id)
  , m_timestamp(0)
  , m_nextBlockSize(0)
  , m_rx(0)
  , m_rxSeq(0)
  , m_tx(0)
  , m_txSeq(0)
{
  SCHAT_DEBUG_STREAM(this)

  m_timer = new QBasicTimer();
  m_txBuffer.reserve(8192);

  m_rxStream = new QDataStream(this);
  m_txStream = new QDataStream(&m_txBuffer, QIODevice::ReadWrite);
  m_sendStream = new QDataStream(&m_sendBuffer, QIODevice::ReadWrite);
  m_readStream = new QDataStream(&m_readBuffer, QIODevice::ReadWrite);

  connect(this, SIGNAL(connected()), SLOT(connected()));
  connect(this, SIGNAL(disconnected()), SLOT(disconnected()));
  connect(this, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(error(QAbstractSocket::SocketError)));
  connect(this, SIGNAL(readyRead()), SLOT(readyRead()));

  #if !defined(SCHAT_NO_SSL)
  m_sslAvailable = QSslSocket::supportsSsl();
  connect(this, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
  connect(this, SIGNAL(encrypted()), SLOT(encrypted()));
  #endif
}


SimpleSocket::~SimpleSocket()
{
  SCHAT_DEBUG_STREAM("~" << this)

  if (m_timer->isActive())
    m_timer->stop();

  delete m_timer;
  delete m_rxStream;
  delete m_txStream;
  delete m_sendStream;
  delete m_readStream;
}


bool SimpleSocket::send(const QByteArray &packet)
{
  SCHAT_DEBUG_STREAM(this << "send(...)")

  if (packet.size() > 65535)
    return transmit(packet, Protocol::HugePackets);

  return transmit(packet);
}


/*!
 * Отправка виртуальных пакетов.
 */
bool SimpleSocket::send(const QList<QByteArray> &packets)
{
  SCHAT_DEBUG_STREAM(this << "send(...)" << packets.size());

  bool huge = false;
  for (int i = 0; i < packets.size(); ++i) {
    if (packets.at(i).size() > 65535) {
      huge = true;
      break;
    }
  }

  if (huge)
    return transmit(packets, Protocol::HugePackets);

  return transmit(packets);
}


/*!
 * Установка дескриптора сокета, данная функция вызывается только для серверного сокета.
 */
bool SimpleSocket::setSocketDescriptor(int socketDescriptor)
{
  if (QSslSocket::setSocketDescriptor(socketDescriptor)) {
    m_serverSide = true;
    #if QT_VERSION >= 0x040600
    setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    #endif
    setTimerState(WaitingHandshake);

    #if !defined(SCHAT_NO_SSL)
    if (m_sslAvailable) {
      setSslConfiguration(QSslConfiguration::defaultConfiguration());
      if (sslConfiguration().localCertificate().isNull() || sslConfiguration().privateKey().isNull()) {
        m_sslAvailable = false;
      }
    }
    #endif

    return true;
  }

  return false;
}


void SimpleSocket::leave()
{
  SCHAT_DEBUG_STREAM(this << "leave()")

  if (state() == SimpleSocket::ConnectedState) {
    disconnectFromHost();
    if (!m_serverSide && state() != SimpleSocket::UnconnectedState) {
      if (!waitForDisconnected(1000))
        abort();
    }
  }
  else {
    abort();
    release();
  }
}


void SimpleSocket::setAuthorized(const QByteArray &userId)
{
  if (userId.isEmpty()) {
    m_authorized = false;
    return;
  }

  m_userId = userId;
  m_authorized = true;
  setTimerState(Idling);
}


void SimpleSocket::newPacketsImpl()
{
  emit newPackets(m_id, m_readQueue);
}


void SimpleSocket::timerEvent(QTimerEvent *event)
{
  if (event->timerId() == m_timer->timerId()) {
    SCHAT_DEBUG_STREAM(this << "timerEvent()" << m_timerState)

    if (m_timerState == WaitingConnect || (m_timerState == WaitingHandshake && !m_authorized) || (m_timerState == Idling && m_serverSide) || m_timerState == WaitingReply) {
      setTimerState(Leaving);
      leave();
      return;
    }

    // Отправка пустого транспортного пакета, сервер должен ответить на него таким же пустым пакетом.
    transmit(QByteArray(), Protocol::ContainsInternalPacket);
    setTimerState(WaitingReply);
    return;
  }

  QSslSocket::timerEvent(event);
}


void SimpleSocket::connectToHostImplementation(const QString &hostName, quint16 port, OpenMode openMode)
{
  SCHAT_DEBUG_STREAM(this << "connectToHostImplementation()" << hostName << port)

  m_serverSide = false;
  m_release = false;
  setTimerState(WaitingConnect);

  QSslSocket::connectToHostImplementation(hostName, port, openMode);
}


void SimpleSocket::connected()
{
  SCHAT_DEBUG_STREAM(this << "connected()")
  #if QT_VERSION >= 0x040600
  setSocketOption(QAbstractSocket::KeepAliveOption, 1);
  #endif
  setTimerState(WaitingHandshake);

  if (m_sslAvailable) {
    PacketWriter writer(m_sendStream, Protocol::ProbeSecureConnectionPacket);
    writer.put(quint16(Protocol::SecureConnectionRequest));
    transmit(writer.data(), Protocol::ContainsInternalPacket);
  }
  else {
    emit requestAuth(m_id);
  }
}


void SimpleSocket::disconnected()
{
  SCHAT_DEBUG_STREAM(this << "disconnected()" << state())
  release();
}


void SimpleSocket::error(QAbstractSocket::SocketError socketError)
{
  SCHAT_DEBUG_STREAM(this << "error()" << socketError << state())

  Q_UNUSED(socketError)

  if (state() != QSslSocket::ConnectedState)
    release();
}


/*!
 * Слот вызывается при получении новых данных.
 * Если данных достаточно для чтения транспортного пакета происходит чтение пакета.
 *
 * \sa readTransport()
 */
void SimpleSocket::readyRead()
{
  SCHAT_DEBUG_STREAM(this << "readyRead()" << bytesAvailable())

  forever {
    if (!m_nextBlockSize) {
      if (bytesAvailable() < 4)
        break;

      *m_rxStream >> m_nextBlockSize;
    }

    if (bytesAvailable() < m_nextBlockSize)
      break;

    if (!readTransport())
      read(m_nextBlockSize);

    m_nextBlockSize = 0;
  }

  if (m_readQueue.isEmpty())
    return;

  if (m_serverSide)
    setTimerState(Idling);

  // Отправка подтверждения о доставке.
  if (m_serverSide && !m_deliveryConfirm.isEmpty()) {
    PacketWriter writer(m_sendStream, Protocol::DeliveryConfirmationPacket);
    writer.put<quint16>(0);
    writer.put(m_deliveryConfirm);
    transmit(writer.data(), Protocol::ContainsInternalPacket);
    m_deliveryConfirm.clear();
  }

  newPacketsImpl();
  m_readQueue.clear();
}


#if !defined(SCHAT_NO_SSL)
void SimpleSocket::encrypted()
{
  SCHAT_DEBUG_STREAM(this << "encrypted()")
  emit requestAuth(m_id);
}


void SimpleSocket::sslErrors(const QList<QSslError> &errors)
{
  SCHAT_DEBUG_STREAM(this << "sslErrors()" << errors)

  QList<QSslError::SslError> noCriticalErrors;
  noCriticalErrors.append(QSslError::NoError);
  noCriticalErrors.append(QSslError::SelfSignedCertificate);
  noCriticalErrors.append(QSslError::HostNameMismatch);

  for (int i = 0; i < errors.size(); ++i) {
    if (!noCriticalErrors.contains(errors.at(i).error()))
      return;
  }

  ignoreSslErrors();
}
#endif


/*!
 * Чтение транспортного пакета.
 */
bool SimpleSocket::readTransport()
{
  SCHAT_DEBUG_STREAM(this << "readTransport()")

  m_rx += m_nextBlockSize + 4;
  TransportReader reader(m_nextBlockSize, m_rxStream);
  int type = reader.readHeader();
  m_nextBlockSize = reader.available();

  if (type == Protocol::GenericTransport) {
    m_rxSeq = reader.sequence();

    int options = reader.options();
    if (options & Protocol::TimeStamp)
      m_timestamp = reader.timestamp();

    // Чтение служебного транспортного пакета.
    if (options & Protocol::ContainsInternalPacket) {
      if (m_serverSide)
        setTimerState(Idling);

      if (m_nextBlockSize == 0) {
        if (m_serverSide)
          transmit(QByteArray(), Protocol::ContainsInternalPacket);
        else
          setTimerState(Idling);
      }

      m_readBuffer = reader.readOne();
      PacketReader packet(m_readStream);

     // Подтверждение доставки.
      if (!m_serverSide && packet.type() == Protocol::DeliveryConfirmationPacket) {
        setTimerState(Idling);
        packet.get<quint16>();
        QList<quint64> list = packet.get<QList<quint64> >();

        if (!list.isEmpty()) {
          foreach (quint64 key, list) {
            m_deliveryConfirm.removeAll(key);
          }

          if (m_deliveryConfirm.isEmpty())
            emit allDelivered(m_id);
        }

        SCHAT_DEBUG_STREAM(this << "DeliveryConfirmation" << list << "r. size:" << m_deliveryConfirm.size());
      }
      else if (packet.type() == Protocol::ProbeSecureConnectionPacket) {
        sslHandshake(packet.get<quint16>());
      }

      reader.skipAll();
      return true;
    }

    /// Из стандартного транспортного пакета, виртуальные пакеты извлекаются в очередь \p m_readQueue,
    /// и если это серверный сокет sequence транспортного пакета будет добавлен в \p m_deliveryConfirm.
    m_readQueue += reader.read();

    if (m_serverSide)
      m_deliveryConfirm += m_rxSeq;

    return true;
  }

  return false;
}


bool SimpleSocket::transmit(const QByteArray &packet, quint8 options, quint8 type, quint8 subversion)
{
  return transmit(QList<QByteArray>() += packet, options, type, subversion);
}


/*!
 * Низкоуровневая функция отправки данных, виртуальные пакеты автоматически упаковываются в транспортный пакет.
 *
 * \param packets    Список виртуальных пакетов для отправки.
 * \param options    packet options.
 * \param type       packet type.
 * \param subversion packet subversion.
 * \param version    packet version.
 *
 * \return true если данные были записаны в сокет.
 */
bool SimpleSocket::transmit(const QList<QByteArray> &packets, quint8 options, quint8 type, quint8 subversion)
{
  SCHAT_DEBUG_STREAM(this << "transmit(...)")
  SCHAT_DEBUG_STREAM(this << "  >> seq:" << m_txSeq << "opt:" << options << "type:" << type << "sv:" << subversion << "v:" << version << "r. size:" << m_deliveryConfirm.size());
  if (!isReady())
    return false;

  if (packets.isEmpty())
    return false;

  qint64 timestamp = 0;
  if (m_serverSide && options != Protocol::ContainsInternalPacket)
    timestamp = m_timestamp;

  TransportWriter tp(m_txStream, packets, m_txSeq, timestamp, options, type, subversion);
  QByteArray packet = tp.data();

  if (!m_serverSide && options != Protocol::ContainsInternalPacket) {
    m_deliveryConfirm.append(m_txSeq);
    m_txSeq++;
    setTimerState(WaitingReply);
  }

  int size = write(packet);
  if (size == -1)
    return false;

  m_tx += size;
  SCHAT_DEBUG_STREAM(this << "  >> Transmit:" << size << "Total:" << m_tx << "Receive:" << m_rx)
  return true;
}



void SimpleSocket::release()
{
  SCHAT_DEBUG_STREAM(this << "release()" << m_release);

  if (m_release)
    return;

  m_release = true;

  if (m_timerState == Leaving) {
    setErrorString(tr("Time out"));
  }

  if (m_timer->isActive())
    m_timer->stop();

  m_sendBuffer.clear();
  m_readBuffer.clear();
  m_txBuffer.clear();
  m_deliveryConfirm.clear();

  emit released(m_id);
}


/*!
 * Установка состояния таймера.
 */
void SimpleSocket::setTimerState(TimerState state)
{
  SCHAT_DEBUG_STREAM(this << "setTimerState()" << state);

  if ((state == Idling || state == WaitingReply) && !m_authorized)
    return;

  m_timerState = state;
  if (m_timer->isActive())
    m_timer->stop();

  switch (state) {
    case WaitingConnect:
      m_timer->start(Protocol::ConnectTime, this);
      break;

    case WaitingHandshake:
      m_timer->start(Protocol::HandshakeTime, this);
      break;

    case Idling:
      if (m_authorized) {
        if (m_serverSide)
          m_timer->start(Protocol::MaxServerIdleTime, this);
        else
          m_timer->start(Protocol::IdleTime, this);
      }
      break;

    case WaitingReply:
      m_timer->start(Protocol::ReplyTime, this);
      break;

    case Leaving:
      break;
  }
}


/*!
 * Обработка запроса клиента на шифрованное подключение,
 * в случае поддержкой сервером шифрования, он будет установлено.
 */
void SimpleSocket::sslHandshake(int option)
{
  if (option == Protocol::SecureConnectionRequest && m_serverSide) {
    quint16 answer = Protocol::SecureConnectionUnavailable;
    if (m_sslAvailable)
      answer = Protocol::SecureConnectionAvailable;

    PacketWriter writer(m_sendStream, Protocol::ProbeSecureConnectionPacket);
    writer.put(answer);
    transmit(writer.data(), Protocol::ContainsInternalPacket);

    #if !defined(SCHAT_NO_SSL)
    if (m_sslAvailable)
      startServerEncryption();
    #endif
  }
  else if (option == Protocol::SecureConnectionAvailable && !m_serverSide) {
    #if !defined(SCHAT_NO_SSL)
    startClientEncryption();
    #endif
  }
  else if (option == Protocol::SecureConnectionUnavailable && !m_serverSide) {
    emit requestAuth(m_id);
  }
}
