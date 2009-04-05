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

#define SCHAT_DEBUG

#include "simpleclienthandler.h"
#include "asio/asio.hpp"
#include "connection.h"
#include "protocol4.h"

/*!
 * \brief Приватный D-класс для класса Connection.
 */
class Connection::Private
{
public:
  Private(Connection *parent, asio::io_service &ioService)
  : handler(0),
  checkTimer(0),
  pingTimer(ioService),
  socket(ioService),
  q(parent),
  pingState(Connection::WaitPing),
  bodySize(0),
  opcode(0),
  m_state(WaitGreeting)
  {}

  ~Private()
  {
    if (handler) delete handler;
    if (checkTimer) delete checkTimer;
  }

  inline State state() const        { return m_state; }
  inline void setState(State state) { m_state = state; }
  void check(asio::error_code &err);
  void close();
  void readBody(const asio::error_code &err, int bytes);
  void readHeader(const asio::error_code &err, int bytes);
  void send();
  void start();

  AbstractProtocolHandler *handler;                ///< Обработчик протокола.
  asio::deadline_timer *checkTimer;                ///< Таймер, обслуживающий соединение.
  asio::deadline_timer pingTimer;                  ///< Таймер, для пинг-запросов для состояния соединения.
  asio::ip::tcp::socket socket;                    ///< Сокет обслуживающий данное соединение.
  char bodyBuffer[protocol::packet::MaxSize];      ///< Буфер для чтения тела пакета.
  char headerBuffer[protocol::packet::HeaderSize]; ///< Буфер для заголовка пакета.
  char sendBuffer[protocol::packet::MaxSize];      ///< Буфер отправки пакета.
  Connection *q;                                   ///< Указатель на родительский объект.
  PingState pingState;                             ///< Текущее состояние механизма проверки соединения.
  QQueue<QByteArray> sendQueue;                    ///< Очередь пакетов для отправки.
  quint16 bodySize;                                ///< Размер тела пакета.
  quint16 opcode;                                  ///< Опкод пакета.

private:
  bool detect();

  State m_state;                                   ///< Статус соединения.
};


/*!
 * Обнаружение мёртвых соединений без инициированного рукопожатия.
 * Функция вызывается спустя \b 20 секунд.
 *
 * Если рукопожатие не было инициировано соединение разрывается.
 * В противном случае запускается пинг-таймер.
 *
 * \param err Код ошибки таймера.
 */
void Connection::Private::check(asio::error_code &err)
{
  DEBUG_OUT("Connection::Private::check()" << this)
//  qDebug() << "[2]" << QDateTime::currentDateTime().toString("mm:ss") << state();

  if (!err) {
//    if (d->state() == WaitGreeting) {
//      handleClose();
//      return;
//    }

//    if (d->state() != WaitClose) {
//      d->timer.expires_from_now(boost::posix_time::millisec(50));
//      d->timer.async_wait(boost::bind(&Connection::checkGreeting, shared_from_this(), _1));
//    }
//      startPing(WaitPing, schat::PingInterval);
  }

  if (checkTimer) {
    delete checkTimer;
    checkTimer = 0;
  }
}


/*!
 * Закрытие соединения.
 * Состояние устанавливается в Connection::WaitClose, останавливается
 * таймер и останавливается передача и приём данных и сокет закрывается.
 *
 * \todo Необходимо уведомлять ядро о причине разъединения.
 */
void Connection::Private::close()
{
  if (state() == WaitClose)
    return;

  setState(WaitClose);
  if (handler)
    handler->reset();

  asio::error_code ignored;
  if (checkTimer)
    checkTimer->cancel(ignored);

  socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignored);
  socket.close(ignored);
}


/*!
 * Чтение тела пакета.
 */
void Connection::Private::readBody(const asio::error_code &err, int bytes)
{
//  qDebug() << "readBody()" << this;
//  qDebug() << "  Transferred:     " << bytes << "bytes";
//  qDebug() << "  RAW Body:        " << QByteArray(bodyBuffer, bodySize).toHex();

  if (!err) {
    if (bytes != bodySize) {
      close();
      return;
    }

    handler->append(opcode, QByteArray(bodyBuffer, bodySize));

//    switch (d->opcode) {
//      case OpcodeGreeting:
//        {
//          quint16 err = opcodeGreeting();
//          if (err) {
//            m_sendQueue.clear();
//            m_sendQueue.enqueue(Packet::create(OpcodeAccessDenied, err));
//            send();
//            handleClose();
//            return;
//          }
//        }
//        m_oldProtocol = true;
//        break;
//    }

//    if (!m_oldProtocol)
//      startPing(WaitPing, schat::PingInterval);

    socket.async_read_some(asio::buffer(headerBuffer, protocol::packet::HeaderSize),
        boost::bind(&Connection::handleReadHeader, q->shared_from_this(), _1, _2));
  }
  else
    close();
}


/*!
 * Чтение заголовка пакета.
 * Размер пакета записывается в \a d->bodySize, тип пакета в \a d->opcode.
 *
 * \param err   Код ошибки.
 * \param bytes Число полученных байт.
 */
void Connection::Private::readHeader(const asio::error_code &err, int bytes)
{
  qDebug() << "readHeader()" << this;
  qDebug() << "  Transferred:     " << bytes << "bytes";
  qDebug() << "  RAW Header:      " << QByteArray(headerBuffer, protocol::packet::HeaderSize).toHex();

  if (!err) {
    using namespace protocol;

    if (bytes != packet::HeaderSize) {
      close();
      return;
    }

    QByteArray header(headerBuffer, packet::HeaderSize);
    QDataStream stream(&header, QIODevice::ReadOnly);
    stream.setVersion(protocol::StreamVersion);
    stream >> bodySize >> opcode;

    if (bodySize > (packet::MaxSize - packet::HeaderSize)) {
      close();
      return;
    }

    if (state() == WaitGreeting) {
      if (!detect()) {
        close();
        return;
      }
      else
        setState(WaitAccess);
    }
    else if (state() != Ready) {
      close();
      return;
    }

//    if (state() != Ready) {
//      if (state() == WaitGreeting && d->opcode == OpcodeGreeting)
//        state(WaitAccess);
//      else if ((state() == WaitGreeting && d->opcode != OpcodeGreeting) || state() == WaitAccess) {
//        handleClose();
//        return;
//      }
//    }

//    qDebug() << "  Full Packet Size:" << d->bodySize + 2 << "bytes";
//    qDebug() << "  Packet Opcode:   " << d->opcode;

    if (bodySize == 0) {
//      if (d->opcode == OpcodePong) {
//        qDebug() << "<<<< PONG OK <<<<" << QDateTime::currentDateTime().toString("[mm:ss]");
//        startPing(WaitPing, schat::PingInterval);
//      }

      socket.async_read_some(asio::buffer(headerBuffer, packet::HeaderSize),
          boost::bind(&Connection::handleReadHeader, q->shared_from_this(), _1, _2));
    }
    else {
      socket.async_read_some(asio::buffer(bodyBuffer, bodySize),
          boost::bind(&Connection::handleReadBody, q->shared_from_this(), _1, _2));
    }
  }
  else
    close();
}


/*!
 * Отправка пакета из очереди.
 * В случае если очередь sendQueue не пуста, то извлекается следующий пакет
 * и записывается в буфер отправки, после этого инициализируется асинхронная
 * запись в сокет.
 *
 * \todo Добавить склейку пакетов.
 */
void Connection::Private::send()
{
//  qDebug() << this << "send()" << QThread::currentThread();

  if (!sendQueue.isEmpty()) {
    QByteArray data = sendQueue.dequeue();
    QDataStream out(&data, QIODevice::ReadOnly);
    out.setVersion(protocol::StreamVersion);
    out.device()->read(sendBuffer, data.size());

//    qDebug() << "  RAW Send:" << QByteArray(m_send, data.size()).toHex();

    socket.async_write_some(asio::buffer(sendBuffer, data.size()),
        boost::bind(&Connection::handleWrite, q->shared_from_this(), _1, _2));
  }
}


/*!
 * Запуск первой асинхронной операции для этого соединения.
 */
void Connection::Private::start()
{
  if (!checkTimer) checkTimer = new asio::deadline_timer(socket.get_io_service());
  checkTimer->expires_from_now(boost::posix_time::seconds(protocol::CheckTimeout));
  checkTimer->async_wait(boost::bind(&Connection::handleCheck, q->shared_from_this(), _1));

  socket.async_read_some(asio::buffer(headerBuffer, protocol::packet::HeaderSize),
      boost::bind(&Connection::handleReadHeader, q->shared_from_this(), _1, _2));
}


bool Connection::Private::detect()
{
  if (opcode == protocol::Greeting) {
    handler = new SimpleClientHandler(q->shared_from_this());
    return true;
  }
  return false;
}


/*!
 * Construct a connection with the given io_service.
 */
Connection::Connection(asio::io_service &ioService)
  : d(new Private(this, ioService))
{
  qDebug() << "Connection()" << this;

//  qRegisterMetaType<UserData>();
//  connect(this, SIGNAL(greeting(const UserData &)), m_daemon, SLOT(greeting(const UserData &)),  Qt::QueuedConnection);
//  connect(this, SIGNAL(leave(const QString &)),     m_daemon, SLOT(localLeave(const QString &)), Qt::QueuedConnection);
}


Connection::~Connection()
{
  qDebug() << "~" << this;
  delete d;
}

/*!
 * Получение сокета ассоциированного с этим соединением.
 */
asio::ip::tcp::socket& Connection::socket()
{
  return d->socket;
}


/*!
 * Закрытие соединения.
 *
 * \note Эта функция потокобезопастна.
 */
void Connection::close()
{
//  m_strand.post(boost::bind(&Connection::handleClose, shared_from_this()));
}


/*!
 * Принудительное закрытие соединения.
 */
void Connection::die()
{
  d->close();
}


/*!
 * Устанавливает состояние соединения в Connection::Ready, что означает
 * успешное прохождение авторизации.
 *
 * \note Эта функция потокобезопастна.
 */
void Connection::ready()
{
  d->setState(Ready);
}


/*!
 * Отправка пакета.
 * Пакет добавляется в очередь Connection::m_sendQueue и если она была пуста
 * немедленно производится начало асинхронной отправки.
 *
 * \param data Полностью сформированный пакет.
 *
 * \note Эта функция потокобезопастна, процесс отправки будет выполнен
 * в контексте этого объекта.
 */
void Connection::send(const QByteArray &data)
{
//  qDebug() << this << "send(const QByteArray &)" << QThread::currentThread();

//  d->mutex.lock();
//  d->sendQueue.enqueue(data);
//  if (d->sendQueue.size() == 1)
//    m_strand.post(boost::bind(&Connection::send, shared_from_this()));

//  d->mutex.unlock();
}


/*!
 * \sa Connection::Private::start().
 */
void Connection::start()
{
  d->start();
}


/*!
 * Обработка пакета \b OpcodeGreeting.
 * Данные извлекаются из пакета для принятия решения о досрочном отказе
 * в доступе, в случае неверных данных.
 *
 * \return \a true в случае успешной предварительной проверки.
 */
quint16 Connection::opcodeGreeting()
{
//  QByteArray body(d->bodyBuffer, d->bodySize - 2);
//  QDataStream stream(&body, QIODevice::ReadOnly);
//  stream.setVersion(StreamVersion);
//
//  quint16 p_version;
//  stream >> p_version;
//  if (p_version < 3)
//    return ErrorOldClientProtocol;
//  else if (p_version > 3)
//    return ErrorOldServerProtocol;
//
//  quint8 p_flag;
//  stream >> p_flag;
//  if (p_flag != FlagNone)
//    return ErrorBadGreetingFlag;
//
//  UserData out;
//
//  stream >> out.gender >> out.nick;
//  out.nick = UserTools::nick(out.nick);
//  if (!UserTools::isValidNick(out.nick))
//    return ErrorBadNickName;
//
//  if (m_daemon->isLocalUser(out.nick))
//    return ErrorNickAlreadyUse;
//
//  stream >> out.fullName >> out.userAgent;
//
//  out.fullName = UserTools::fullName(out.fullName);
//  out.userAgent = UserTools::userAgent(out.userAgent);
//  if (!UserTools::isValidUserAgent(out.userAgent))
//    return ErrorBadUserAgent;
//
//  stream >> out.byeMsg;
//  out.byeMsg = UserTools::byeMsg(out.byeMsg);
//  m_nick = out.nick;
//  out.protocol = 3;
//  out.host = d->socket.remote_endpoint().address().to_string().c_str();
//
//  qDebug() << "           " << m_nick;

  return 0;
}


/*!
 * \sa Connection::Private::check(asio::error_code &err).
 */
void Connection::handleCheck(asio::error_code &err)
{
  d->check(err);
}


/*!
 * \sa Connection::Private::close().
 */
void Connection::handleClose() { d->close(); }


/*!
 * \sa Connection::Private::readBody(const asio::error_code &err, int bytes).
 */
void Connection::handleReadBody(const asio::error_code &err, int bytes)
{
  d->readBody(err, bytes);
}


/*!
 * \sa Connection::Private::readHeader(const asio::error_code &err, int bytes).
 */
void Connection::handleReadHeader(const asio::error_code &err, int bytes)
{
  d->readHeader(err, bytes);
}


/*!
 * Handle completion of a write operation.
 */
void Connection::handleWrite(const asio::error_code &err, int bytes)
{
  Q_UNUSED(bytes)

  if (err)
    d->close();
  else
    d->send();
}


/*!
 * Обработка событий от пинг-таймера.
 * Если текущее состояние равно Connection::WaitPing, то отправляется пинг-пакет
 * и устанавливается состояние ожидания понг-пакета, если понг-таймер
 * не будет сброшен пришедшим понг-пакетом или в случае использования
 * нового протокола любым входящим пакетом, то соединение будет разорвано.
 */
void Connection::ping(asio::error_code &e)
{
  if (!e) {
//    if (d->pingState == WaitPing) {
////      qDebug() << ">>>> PING SEND >>>>" << QDateTime::currentDateTime().toString("[mm:ss]");
//      send(Packet::create(OpcodePing));
//      startPing(WaitPong, schat::PongTimeout);
//    }
//    else {
////      qDebug() << "<<<< PONG FAIL >>>>" << QDateTime::currentDateTime().toString("[mm:ss]");
//      handleClose();
//    }
  }
}


void Connection::startPing(PingState state, int sec)
{
//  if (sec < 1)
//    sec = schat::PingInterval;

  d->pingState = state;
//  d->timer.expires_from_now(boost::posix_time::seconds(sec));
//  d->timer.async_wait(boost::bind(&Connection::ping, shared_from_this(), _1));
}
