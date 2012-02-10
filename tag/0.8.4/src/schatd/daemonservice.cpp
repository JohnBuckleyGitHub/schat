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

#include <QtCore>
#include <QtNetwork>

#include "abstractprofile.h"
#include "channellog.h"
#include "daemonservice.h"
#include "text/PlainTextFilter.h"

#ifdef SCHAT_DEBUG
  #undef SCHAT_DEBUG
  #define SCHAT_DEBUG(x) qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << x;
  #include <QDebug>
  #include <QTime>
#else
  #define SCHAT_DEBUG(x)
#endif

/*!
 * \brief Конструктор класса DaemonService.
 */
DaemonService::DaemonService(QTcpSocket *socket, QObject *parent)
: QObject(parent), m_socket(socket)
{
  SCHAT_DEBUG(this);

  if (m_socket) {
    m_socket->setParent(this);
    connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(m_socket, SIGNAL(disconnected()), SLOT(disconnected()));
    m_accepted = false;
    m_nextBlockSize = 0;
    m_stream.setDevice(m_socket);
    m_stream.setVersion(StreamVersion);
    m_pings = 0;
    m_ping.start(9000);
    m_numeric = 0;
    m_kill = false;
    connect(&m_ping, SIGNAL(timeout()), SLOT(ping()));
  }
  else
    deleteLater();
}


/*!
 * Возвращает \a true если сервис находится в активном состоянии.
 */
bool DaemonService::isReady() const
{
  if (m_socket) {
    if (m_socket->state() == QTcpSocket::ConnectedState && m_accepted)
      return true;
    else
      return false;
  }
  else
    return false;
}


QString DaemonService::host() const
{
  return m_socket->peerAddress().toString();
}


/*!
 * Клиент получил отказ в доступе, `quint16 reason` - причина отказа.
 * Отсылаем ошибку и разрываем соединение.
 */
void DaemonService::accessDenied(quint16 reason)
{
  send(OpcodeAccessDenied, reason);
  m_socket->disconnectFromHost();
}


/*!
 * Клиент успешно получил доступ, отсылаем уведомление об успешном доступе
 * и устанавливаем флаг `m_accepted` в `true`.
 */
void DaemonService::accessGranted(quint16 numeric)
{
  if (!m_accepted) {
    send(OpcodeAccessGranted, numeric);
    m_accepted = true;
  }
}


/*!
 *
 */
void DaemonService::quit(bool kill)
{
  m_kill = kill;

  if (isReady())
    m_socket->disconnectFromHost();
}


/** [public]
 * Пакет `OpcodeSyncNumerics`.
 */
void DaemonService::sendNumerics(const QList<quint8> &numerics)
{
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << OpcodeSyncNumerics << numerics;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
  }
}


/*!
 * Отправка универсального пакета.
 *
 * \param sub   Субопкод.
 * \param data1 Список данных типа quint32
 * \param data2 Список данных типа QString
 */
bool DaemonService::sendUniversal(quint16 sub, const QList<quint32> &data1, const QStringList &data2)
{
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << OpcodeUniversal << sub << data1 << data2;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
}


/*!
 * \brief Отправка пакета с опкодом \b OpcodeNewNick.
 *
 */
void DaemonService::sendNewNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name)
{
  SCHAT_DEBUG(this << "::sendNewNick()")

  if (m_profile->nick() == nick && m_flag == FlagNone) {
    m_profile->setGender(gender);
    m_profile->setNick(newNick);
    m_profile->setFullName(name);
  }
  send(OpcodeNewNick, gender, nick, newNick, name);
}


/*!
 * \brief Отправка пакета с опкодом \b OpcodeNewProfile.
 *
 */
void DaemonService::sendNewProfile(quint8 gender, const QString &nick, const QString &name)
{
  SCHAT_DEBUG(this << "::sendNewProfile()")

  if (m_profile->nick() == nick) {
    m_profile->setGender(gender);
    m_profile->setFullName(name);
  }
  send(OpcodeNewProfile, gender, nick, name);
}


/** [public slots]
 * Формирует и отправляет пакет с опкодом `OpcodeNewUser`.
 */
void DaemonService::sendNewUser(const QStringList &list, quint8 echo, quint8 numeric)
{
  if (isReady()) {

    if (m_flag == FlagNone)
      if (list.at(AbstractProfile::Nick) == m_profile->nick() && !echo)
        return;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0)
        << OpcodeNewUser
        << echo
        << numeric
        << AbstractProfile::genderNum(list.at(AbstractProfile::Gender))
        << list.at(AbstractProfile::Nick)
        << list.at(AbstractProfile::FullName)
        << list.at(AbstractProfile::ByeMsg)
        << list.at(AbstractProfile::UserAgent)
        << list.at(AbstractProfile::Host);

    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
  }
}


/** [public slots]
 *
 */
void DaemonService::sendRelayMessage(const QString &channel, const QString &sender, const QString &message)
{
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0)
        << OpcodeRelayMessage
        << channel
        << sender
        << message;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
  }
}


/*!
 * Обработка разрыва соединения.
 */
void DaemonService::disconnected()
{
  if (m_accepted && !m_kill) {
    if (m_error.isEmpty())
      m_error = m_socket->errorString();

    emit leave(m_profile->nick(), m_flag, m_error);
  }

  deleteLater();
}


/*!
 * Слот вызывается таймером \a m_ping, для проверки состояния соединения.
 *
 * Если соединение успешно установлено (m_accepted == true) и счётчик отправленных пакетов \a m_pings
 * не превышен, производится отправка пакета \b OpcodePing и увеличивается счётчик пингов,
 * иначе \a m_error устанавливается в "Ping timeout" и происходит отключение.
 *
 * При (m_accepted == false) при активном соединения происходит разъединение, иначе удаление сокета.
 */
void DaemonService::ping()
{
  if (m_accepted) {
    if (m_pings < 2) {
      send(OpcodePing);
      ++m_pings;
    }
    else {
      m_error = tr("Ping timeout");
      m_socket->disconnectFromHost();
    }
  }
  else {
    if (m_socket->state() == QAbstractSocket::ConnectedState)
      m_socket->disconnectFromHost();
    else
      deleteLater();
  }
}


/** [public slots]
 *
 */
void DaemonService::readyRead()
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

    if (m_opcode != 401) {
      SCHAT_DEBUG(this << "opcode:" << m_opcode << "size:" << m_nextBlockSize)
    }

    if (m_accepted) {
      switch (m_opcode) {
        case OpcodeMessage:
          opcodeMessage();
          break;

        case OpcodePong:
          opcodePong();
          break;

        case OpcodeNewProfile:
          opcodeNewProfile();
          break;

        case OpcodeByeMsg:
          opcodeByeMsg();
          break;

        case OpcodeRelayMessage:
          opcodeRelayMessage();
          break;

        case OpcodeNewUser:
          opcodeNewUser();
          break;

        case OpcodeUserLeave:
          opcodeUserLeave();
          break;

        case OpcodeNewNick:
          opcodeNewNick();
          break;

        case OpcodeSyncByeMsg:
          opcodeSyncByeMsg();
          break;

        case OpcodeUniversal:
          opcodeUniversal();
          break;

        default:
          unknownOpcode();
          break;
      };
    }
    else if (m_opcode == OpcodeGreeting) {
      if (!opcodeGreeting()) {
        m_socket->disconnectFromHost();
        return;
      }
    }
    else {
      m_socket->disconnectFromHost();
      return;
    }
  }
}


bool DaemonService::opcodeGreeting()
{
  SCHAT_DEBUG(this << "::opcodeGreeting()")

  quint16 p_version;
  quint8  p_gender;
  QString p_nick;
  QString p_name;
  QString p_userAgent;
  QString p_byeMsg;
  quint16 err;

  m_stream >> p_version >> m_flag >> p_gender >> p_nick >> p_name >> p_userAgent >> p_byeMsg;
  m_nextBlockSize = 0;

  QStringList profile;
  profile << p_nick << p_name << p_byeMsg << p_userAgent << m_socket->peerAddress().toString() << AbstractProfile::gender(p_gender);;
  m_profile = new AbstractProfile(profile, this);

  err = verifyGreeting(p_version);

  if (err) {
    accessDenied(err);
    return false;
  }

  emit greeting(m_profile->pack(), m_flag);

  return true;
}


/*!
 * Отправка стандартного пакета:
 * quint16 -> размер пакета
 * quint16 -> опкод
 * ОПКОДЫ:
 *   `OpcodePing`.
 */
bool DaemonService::send(quint16 opcode)
{
  if (isReady()) {
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


/** [private]
 * Отправка стандартного пакета:
 * quint16 -> размер пакета
 * quint16 -> опкод
 * QString ->
 * ОПКОДЫ:
 *   `OpcodeServerMessage`.
 */
bool DaemonService::send(quint16 opcode, const QString &msg)
{
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << opcode << msg;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
}


/** [private]
 * Отправка стандартного пакета:
 * quint16 -> размер пакета
 * quint16 -> опкод
 * QString ->
 * QString ->
 * ОПКОДЫ:
 *   `OpcodeMessage`.
 */
bool DaemonService::send(quint16 opcode, const QString &str1, const QString &str2)
{
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << opcode << str1 << str2;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
}


/** [private]
 * Отправка стандартного пакета:
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint16 ->
 * ОПКОДЫ:
 *   `OpcodeAccessGranted`, `OpcodeAccessDenied`.
 */
bool DaemonService::send(quint16 opcode, quint16 err)
{
  if (m_socket->state() == QTcpSocket::ConnectedState) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << opcode << err;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
}


/** [private]
 * Отправка стандартного пакета:
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint8  ->
 * QString ->
 * QString ->
 * ОПКОДЫ:
 *   `OpcodePrivateMessage`, `OpcodeUserLeave`.
 */
bool DaemonService::send(quint16 opcode, quint8 flag, const QString &nick, const QString &message)
{
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << opcode << flag << nick << message;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
}


/** [private]
 * Отправка стандартного пакета:
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint8  ->
 * QString ->
 * QString ->
 * QString ->
 * ОПКОДЫ:
 *   `OpcodeNewNick`.
 */
bool DaemonService::send(quint16 opcode, quint8 gender, const QString &nick, const QString &newNick, const QString &name)
{
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << opcode << gender << nick << newNick << name;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
}


/*!
 * Обнаружение команды "/all".
 */
QString DaemonService::parseCmd(const QString &message) const
{
  QString text = PlainTextFilter::filter(message).toLower();
  QString out = message;
  if (text.startsWith("/all ", Qt::CaseInsensitive)) {
    QString cmd = "/all ";
    int index = out.indexOf(cmd, 0, Qt::CaseInsensitive);
    if (index == -1) {
      cmd = "/all";
      index = out.indexOf(cmd, 0, Qt::CaseInsensitive);
    }
    if (index != -1)
      out.remove(index, cmd.size());

    out = "<b>" + m_profile->nick() + "</b> " + out;
  }

  return out;
}


/** [private]
 * Верификация пакета `OpcodeGreeting`.
 */
quint16 DaemonService::verifyGreeting(quint16 version)
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
 * \brief Разбор пакета с опкодом \b OpcodeByeMsg.
 */
void DaemonService::opcodeByeMsg()
{
  QString p_bye;
  m_stream >> p_bye;
  m_nextBlockSize = 0;
  m_profile->setByeMsg(p_bye);
  emit newBye(m_profile->nick(), p_bye);
}


/*!
 * \brief Разбор пакета \b OpcodeMessage, полученного от клиента.
 *
 * В случае успеха высылается сигнал message(const QString &channel, const QString &sender, const QString &message).
 */
void DaemonService::opcodeMessage()
{
  QString p_channel;
  QString p_message;
  m_stream >> p_channel >> p_message;
  m_nextBlockSize = 0;

  SCHAT_DEBUG(this << "::opcodeMessage()")
  SCHAT_DEBUG("  CHANNEL:" << p_channel)
  SCHAT_DEBUG("  SENDER: " << m_profile->nick())
  SCHAT_DEBUG("  MESSAGE:" << p_message)

  if (p_message.isEmpty())
    return;

  p_message = ChannelLog::htmlFilter(p_message);
  if (p_message.isEmpty())
    return;

  emit message(p_channel, m_profile->nick(), p_message);
}


void DaemonService::opcodeNewNick()
{
  SCHAT_DEBUG(this << "::opcodeNewNick()")

  quint8 p_gender;
  QString p_nick;
  QString p_newNick;
  QString p_name;
  m_stream >> p_gender >> p_nick >> p_newNick >> p_name;
  m_nextBlockSize = 0;
  emit newNick(p_gender, p_nick, p_newNick, p_name);
}


/*!
 * \brief Разбор пакета с опкодом \b OpcodeNewProfile.
 *
 *
 */
void DaemonService::opcodeNewProfile()
{
  SCHAT_DEBUG(this << "::opcodeNewProfile()")

  quint8 p_gender;
  QString p_nick;
  QString p_name;
  m_stream >> p_gender >> p_nick >> p_name;
  m_nextBlockSize = 0;

  if (p_nick.isEmpty())
    return;

  if (m_profile->nick() == p_nick)
    emit newProfile(p_gender, m_profile->nick(), p_name);
  else
    emit newNick(p_gender, m_profile->nick(), p_nick, p_name);
}


/** [private]
 *
 */
void DaemonService::opcodeNewUser()
{
  quint8 p_flag;
  quint8 p_numeric;
  quint8 p_gender;
  QString p_nick;
  QString p_name;
  QString p_bye;
  QString p_agent;
  QString p_host;

  m_stream >> p_flag >> p_numeric >> p_gender >> p_nick >> p_name >> p_bye >> p_agent >> p_host;
  m_nextBlockSize = 0;
  QStringList profile;
  profile << p_nick << p_name << p_bye << p_agent << p_host << AbstractProfile::gender(p_gender);

  emit newUser(profile, p_flag, p_numeric);
}


/** [private]
 * Разбор пакета с опкодом `OpcodePong`.
 * Функция сбрасывает счётчик `OpcodePong`.
 */
void DaemonService::opcodePong()
{
  m_nextBlockSize = 0;
  m_pings = 0;
}


/*!
 * \brief Разбор пакета с опкодом \b OpcodeRelayMessage.
 *
 * В случае успеха высылается сигнал void relayMessage(const QString &channel, const QString &sender, const QString &message).
 */
void DaemonService::opcodeRelayMessage()
{
  QString p_channel;
  QString p_sender;
  QString p_message;
  m_stream >> p_channel >> p_sender >> p_message;
  m_nextBlockSize = 0;

  SCHAT_DEBUG(this << "::opcodeRelayMessage()")
  SCHAT_DEBUG(this << "  CHANNEL:" << p_channel)
  SCHAT_DEBUG(this << "  SENDER: " << p_sender)
  SCHAT_DEBUG(this << "  MESSAGE:" << p_message)

  if (p_sender.isEmpty())
    return;

  if (p_message.isEmpty())
    return;

  p_message = ChannelLog::htmlFilter(p_message);
  if (p_message.isEmpty())
    return;

  emit relayMessage(p_channel, p_sender, p_message);
}


/*!
 * \brief Разбор пакета с опкодом \b OpcodeSyncByeMsg.
 */
void DaemonService::opcodeSyncByeMsg()
{
  QString p_nick;
  QString p_msg;
  m_stream >> p_nick >> p_msg;
  m_nextBlockSize = 0;

  if (p_nick.isEmpty())
    return;

  emit newBye(p_nick, p_msg);
}


/*!
 * Разбор универсального пакета.
 */
void DaemonService::opcodeUniversal()
{
  quint16        subOpcode;
  QList<quint32> data1;
  QStringList    data2;
  m_stream >> subOpcode >> data1 >> data2;
  m_nextBlockSize = 0;

  if (m_flag == FlagLink)
    emit universal(subOpcode, data1, data2, m_numeric);
  else
    emit universal(subOpcode, m_profile->nick(), data1, data2);
}


/*!
 * \brief Разбор пакета с опкодом \b OpcodeUserLeave.
 *
 * В конце разбора высылается сигнал userLeave(const QString &, const QString &, bool).
 */
void DaemonService::opcodeUserLeave()
{
  quint8 p_flag;
  QString p_nick;
  QString p_bye;
  m_stream >> p_flag >> p_nick >> p_bye;
  m_nextBlockSize = 0;

  emit userLeave(p_nick, p_bye, p_flag);
}


/*!
 * Функция читает пакет с неизвестным опкодом.
 */
void DaemonService::unknownOpcode()
{
  SCHAT_DEBUG(this << "::unknownOpcode()")
  SCHAT_DEBUG("opcode:" << m_opcode << "size:" << m_nextBlockSize)

  QByteArray block = m_socket->read(m_nextBlockSize - (int) sizeof(quint16));
  m_nextBlockSize = 0;
}
