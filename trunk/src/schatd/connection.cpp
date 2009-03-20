/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include <QtCore>
#include <boost/bind.hpp>

#include "abstractprofile.h"
#include "connection.h"
#include "packet.h"

/*!
 * Construct a connection with the given io_service.
 */
Connection::Connection(asio::io_service &ioService, QObject *parent)
  : QObject(parent),
  m_profile(0),
  m_timer(ioService),
  m_socket(ioService),
  m_oldProtocol(false),
  m_pingState(WaitPing),
  m_bodySize(0),
  m_opcode(0),
  m_numeric(0),
  m_state(WaitGreeting)
{
  qDebug() << "Connection()" << this;
}


Connection::~Connection()
{
  qDebug() << "~" << this;

  if (m_profile)
    delete m_profile;
}

/*!
 * Get the socket associated with the connection.
 */
asio::ip::tcp::socket& Connection::socket()
{
//  qDebug() << "Connection::socket()" << this;
  return m_socket;
}


/*!
 * Отправка пакета.
 * Пакет добавляется в очередь m_sendQueue и если она была пуста
 * немедленно производится начало асинхронной отправки.
 *
 * \param data Полностью сформированный пакет.
 * \return \a true если состояние сокета равно Ready.
 */
bool Connection::send(const QByteArray &data)
{
//  qDebug() << "send(const QByteArray &)" << this;

  if (m_state == Ready) {
    if (m_sendQueue.isEmpty()) {
      m_sendQueue.enqueue(data);
      send();
    }
    else
      m_sendQueue.enqueue(data);

    return true;
  }
  else
    return false;
}


/*!
 * Start the first asynchronous operation for the connection.
 */
void Connection::start()
{
//  qDebug() << "Connection::start()" << this;

  m_timer.expires_from_now(boost::posix_time::seconds(schat::GreetingTimeout));
  m_timer.async_wait(boost::bind(&Connection::checkGreeting, shared_from_this(), _1));

  m_socket.async_read_some(asio::buffer(m_header, schat::headerSize),
      boost::bind(&Connection::handleReadHeader, shared_from_this(),
        asio::placeholders::error,
        asio::placeholders::bytes_transferred));
}


/*!
 * Обработка пакета \b OpcodeGreeting.
 * Данные извлекаются из пакета для принятия решения о досрочном отказе
 * в доступе, в случае неверных данных.
 *
 * \return \a true в случае успешной предварительной проверки.
 */
bool Connection::opcodeGreeting()
{
//  qDebug() << "Connection::opcodeGreeting()";

  quint16 p_version;
  quint8  p_gender;
  QString p_nick;
  QString p_name;
  QString p_userAgent;
  QString p_byeMsg;
  quint16 err;

  QDataStream stream(QByteArray(m_body, m_bodySize - 2));
  stream >> p_version >> m_flag >> p_gender >> p_nick >> p_name >> p_userAgent >> p_byeMsg;

  QStringList profile;
  profile << p_nick << p_name << p_byeMsg << p_userAgent
      << QString(m_socket.remote_endpoint().address().to_string().c_str())
      << AbstractProfile::gender(p_gender);

  m_profile = new AbstractProfile(profile);
  qDebug() << profile;

  err = verifyGreeting(p_version);

  if (err) {
//    accessDenied(err);
    return false;
  }

//  emit greeting(m_profile->pack(), m_flag);

  return true;
}


/*!
 * Предварительная проверка данных пакета \b OpcodeGreeting.
 *
 * \param version Версия протокола.
 * \return \a 0 - в случае успешной проверки, иначе код ошибки,
 * который будет отправлен клиенту.
 */
quint16 Connection::verifyGreeting(quint16 version)
{
  if (version < ProtocolVersion)
    return ErrorOldClientProtocol;

  if (version > ProtocolVersion)
    return ErrorOldServerProtocol;

  if (!(m_flag == FlagNone || m_flag == FlagLink))
    return ErrorBadGreetingFlag;

  if (!m_profile->isValidNick() && m_flag == FlagNone)
    return ErrorBadNickName;

  if (!m_profile->isValidUserAgent())
    return ErrorBadUserAgent;

  if (m_flag == FlagLink) {
    bool ok;
    m_numeric = quint8(m_profile->nick().toInt(&ok));
    if (ok) {
      if (!m_numeric)
        return ErrorBadNumeric;
    }
    else
      return ErrorBadNumeric;
  }

  return 0;
}


/*!
 * Обнаружение мёртвых соединений без инициированного рукопожатия.
 * Функция вызывается спустя \b schat::GreetingTimeout секунд.
 *
 * Если рукопожатие не было инициировано соединение разрывается.
 * В противном случае запускается пинг-таймер.
 *
 * \param e Код ошибки таймера.
 */
void Connection::checkGreeting(asio::error_code &e)
{
  qDebug() << this << "checkGreeting()";

  if (!e) {
    if (m_state == WaitGreeting) {
      close();
      return;
    }

    if (m_state != WaitClose)
      startPing(WaitPing, schat::PingInterval);
  }
}


/*!
 * Закрытие соединения.
 * Останавливается передачи и приём данных и сокет закрывается.
 */
void Connection::close()
{
  qDebug() << this << "close()";

  m_state = WaitClose;

  asio::error_code ignored;
  m_timer.cancel(ignored);
  m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignored);
  m_socket.close(ignored);
}


/*!
 * Чтение тела пакета.
 */
void Connection::handleReadBody(const asio::error_code &e, int bytes)
{
//  qDebug() << "Connection::handleReadBody()" << this;
//  qDebug() << "  Transferred:     " << bytes << "bytes";
//  qDebug() << "  RAW Body:        " << QByteArray(m_body, m_bodySize - 2).toHex();

  if (!e) {
    if (bytes != m_bodySize - 2) {
      close();
      return;
    }

    switch (m_opcode) {
      case OpcodeGreeting:
        if (opcodeGreeting()) {
          m_state = Ready;
          send(Packet::create(OpcodeAccessGranted, 0));
        }
        else {
          close();
          return;
        }
        m_oldProtocol = true;
        break;
    }

    if (!m_oldProtocol)
      startPing(WaitPing, schat::PingInterval);

    m_socket.async_read_some(asio::buffer(m_header, schat::headerSize),
        boost::bind(&Connection::handleReadHeader, shared_from_this(),
          asio::placeholders::error,
          asio::placeholders::bytes_transferred));
  }
  else
    close();
}


/*!
 * Чтение заголовка пакета.
 * Размер пакета записывается в \a m_bodySize, тип пакета в \a m_opcode.
 *
 * \param e     Код ошибки.
 * \param bytes Число полученных байт.
 */
void Connection::handleReadHeader(const asio::error_code &e, int bytes)
{
//  qDebug() << "Connection::handleReadHeader()" << this;
//  qDebug() << "  Transferred:     " << bytes << "bytes";
//  qDebug() << "  RAW Header:      " << QByteArray(m_header, schat::headerSize).toHex();

  if (!e) {
    if (bytes != schat::headerSize) {
      close();
      return;
    }

    QByteArray header(m_header, schat::headerSize);
    QDataStream stream(header);

    stream >> m_bodySize >> m_opcode;
    if (m_bodySize > 8190) {
      close();
      return;
    }

    if (m_state != Ready) {
      if (WaitGreeting && m_opcode == OpcodeGreeting)
        m_state = WaitAccess;
      else if ((m_state == WaitGreeting && m_opcode != OpcodeGreeting) || m_state == WaitAccess) {
        close();
        return;
      }
    }

//    qDebug() << "  Full Packet Size:" << m_bodySize << "bytes";
//    qDebug() << "  Packet Opcode:   " << m_opcode;

    if (m_bodySize == 2) {
      if (m_opcode == OpcodePong) {
//        qDebug() << "<<<< PONG OK <<<<" << QDateTime::currentDateTime().toString("[mm:ss]");
        startPing(WaitPing, schat::PingInterval);
      }

      m_socket.async_read_some(asio::buffer(m_header, schat::headerSize),
          boost::bind(&Connection::handleReadHeader, shared_from_this(),
            asio::placeholders::error,
            asio::placeholders::bytes_transferred));
    }
    else {
      m_socket.async_read_some(asio::buffer(m_body, m_bodySize - 2),
          boost::bind(&Connection::handleReadBody, shared_from_this(),
            asio::placeholders::error,
            asio::placeholders::bytes_transferred));
    }
  }
  else
    close();
}


/*!
 * Handle completion of a write operation.
 */
void Connection::handleWrite(const asio::error_code &e, int bytes)
{
//  qDebug() << "Connection::handleWrite()" << this << bytes;
  if (!e)
    send();
  else
    close();
}


/*!
 * Обработка событий от пинг-таймера.
 * Если текущее состояние равно WaitPing, то отправляется пинг-пакет
 * и устанавливается состояние ожидания понг-пакета, если понг-таймер
 * не будет сброшен пришедшим понг-пакетом или в случае использования
 * нового протокола любым входящим пакетом, то соединение будет разорвано.
 */
void Connection::ping(asio::error_code &e)
{
  if (!e) {
    if (m_pingState == WaitPing) {
//      qDebug() << ">>>> PING SEND >>>>" << QDateTime::currentDateTime().toString("[mm:ss]");
      send(Packet::create(OpcodePing));
      startPing(WaitPong, schat::PongTimeout);
    }
    else {
//      qDebug() << "<<<< PONG FAIL >>>>" << QDateTime::currentDateTime().toString("[mm:ss]");
      close();
    }
  }
}


/*!
 * Отправка пакета из очереди.
 * В случае если очередь m_sendQueue не пуста, то извлекается следующий пакет
 * и записывается в буфер m_send, после этого инициализируется асинхронная
 * запись в сокет.
 */
void Connection::send()
{
//  qDebug() << this << "send()";

  if (!m_sendQueue.isEmpty()) {
    QByteArray data = m_sendQueue.dequeue();
    QDataStream out(&data, QIODevice::ReadOnly);
    out.device()->read(m_send, data.size());

//    qDebug() << "  RAW Send:" << QByteArray(m_send, data.size()).toHex();

    m_socket.async_write_some(asio::buffer(m_send, data.size()),
        boost::bind(&Connection::handleWrite, shared_from_this(),
          asio::placeholders::error,
          asio::placeholders::bytes_transferred));
  }
}


void Connection::startPing(PingState state, int sec)
{
  if (sec < 1)
    sec = schat::PingInterval;

  m_pingState = state;
  m_timer.expires_from_now(boost::posix_time::seconds(sec));
  m_timer.async_wait(boost::bind(&Connection::ping, shared_from_this(), _1));
}
