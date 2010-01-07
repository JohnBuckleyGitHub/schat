/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

#include "abstractprofile.h"
#include "channellog.h"
#include "protocol/CorePackets.h"
#include "schatmacro.h"

/*!
 * Конструктор класса AbstractRawPacket.
 *
 * \param opcode Код пакета.
 */
AbstractRawPacket::AbstractRawPacket(quint16 opcode)
  : m_valid(true),
  m_opcode(opcode)
{
  SCHAT_DEBUG("AbstractRawPacket" << this << "opcode:" << opcode)
}


AbstractRawPacket::~AbstractRawPacket()
{
  SCHAT_DEBUG("~AbstractRawPacket()" << this);
}


/*!
 * Чтение сырого пакета.
 * Функция формирует поток данных, для дальнейшего разбора пакета.
 *
 * \param opcode Код пакета.
 * \param body   Тело пакета.
 */
bool AbstractRawPacket::read(quint16 opcode, const QByteArray &body)
{
  SCHAT_DEBUG(this << "read(" << opcode << ")");
  m_opcode = opcode;
  QDataStream stream(body);
  stream.setVersion(QDataStream::Qt_4_4);
  return readStream(&stream);;
}


QByteArray AbstractRawPacket::data() const
{
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_4);
  out << m_opcode;
  writeStream(&out);
  return block;
}


/*!
 * Данная функция должна быть переопределена в наследниках,
 * для чтения специфичных данных пакета из потока.
 *
 * \param stream Указатель на поток данных.
 * \return \a false в случае ошибки чтения потока.
 */
bool AbstractRawPacket::readStream(QDataStream *stream)
{
  SCHAT_DEBUG(this << "readStream()")
  if (stream->status() == QDataStream::ReadCorruptData)
    return false;

  return true;
}


void AbstractRawPacket::writeStream(QDataStream *stream) const
{
  SCHAT_DEBUG(this << "writeStream()")
  Q_UNUSED(stream)
}


/*!
 * Конструктор класса HandshakeRequest.
 */
HandshakeRequest::HandshakeRequest()
  : AbstractRawPacket(100),
  m_version(3),
  m_flag(FlagNone),
  m_gender(0),
  m_profile(0),
  m_error(ErrorNoError),
  m_numeric(0)
{
}


/*!
 * Конструктор класса HandshakeRequest.
 */
HandshakeRequest::HandshakeRequest(AbstractProfile *profile, quint16 version, Flag flag)
  : AbstractRawPacket(100),
  m_version(version),
  m_flag(flag),
  m_gender(profile->genderNum()),
  m_nick(profile->nick()),
  m_name(profile->fullName()),
  m_agent(profile->userAgent()),
  m_bye(profile->byeMsg()),
  m_profile(0),
  m_error(ErrorNoError),
  m_numeric(0)
{
}


bool HandshakeRequest::readStream(QDataStream *stream)
{
  *stream >> m_version >> m_flag >> m_gender >> m_nick >> m_name >> m_agent >> m_bye;
  SCHAT_DEBUG(">> m_version  =" << m_version)
  SCHAT_DEBUG(">> m_flag     =" << m_flag)
  SCHAT_DEBUG(">> m_gender   =" << m_gender)
  SCHAT_DEBUG(">> m_nick     =" << m_nick)
  SCHAT_DEBUG(">> m_name     =" << m_name)
  SCHAT_DEBUG(">> m_agent    =" << m_agent)
  SCHAT_DEBUG(">> m_bye      =" << m_bye)

  QStringList profile;
  profile << m_nick << m_name << m_bye << m_agent << "" << AbstractProfile::gender(m_gender);
  m_profile = new AbstractProfile(profile);
  m_error = verify();

  return AbstractRawPacket::readStream(stream);
}


/*!
 * Предварительная проверка пакета.
 */
HandshakeRequest::Error HandshakeRequest::verify()
{
  if (m_version < 3)
    return ErrorOldClientProtocol;

  if (m_version > 3)
    return ErrorOldServerProtocol;

//  if (!(m_flag == FlagNone || m_flag == FlagLink))
//    return ErrorBadGreetingFlag;

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

  return ErrorNoError;
}


void HandshakeRequest::writeStream(QDataStream *stream) const
{
  *stream << m_version << m_flag << m_gender << m_nick << m_name << m_agent << m_bye;
}




/*!
 * Конструктор класса MessagePacket.
 */
MessagePacket::MessagePacket()
  : AbstractRawPacket(200)
{
}


/*!
 * Конструктор класса MessagePacket.
 */
MessagePacket::MessagePacket(const QString &channel, const QString &message)
  : AbstractRawPacket(200),
  m_channel(channel),
  m_message(message)
{
}


/*!
 * - m_channel если не является пустым, должен быть валидным ником.
 * - m_message обрабатывается HTML фильтром и не должен быть пустым.
 */
bool MessagePacket::readStream(QDataStream *stream)
{
  *stream >> m_channel >> m_message;
  SCHAT_DEBUG(">> m_channel  =" << m_channel)
  SCHAT_DEBUG(">> m_message  =" << m_message)

  if (!m_channel.isEmpty()) {
    if (!AbstractProfile::isValidNick(m_channel))
      return false;
  }

  if (m_message.isEmpty())
    return false;

  m_message = ChannelLog::htmlFilter(m_message);
  if (m_message.isEmpty())
    return false;

  return AbstractRawPacket::readStream(stream);
}


void MessagePacket::writeStream(QDataStream *stream) const
{
  *stream << m_channel << m_message;
}


/*!
 * Конструктор класса MessagePacket.
 */
ByeMsgPacket::ByeMsgPacket()
  : AbstractRawPacket(603)
{
}


ByeMsgPacket::ByeMsgPacket(const QString &bye)
  : AbstractRawPacket(603),
  m_bye(bye)
{
}


bool ByeMsgPacket::readStream(QDataStream *stream)
{
  *stream >> m_bye;
  SCHAT_DEBUG(">> m_bye      =" << m_bye)

  return AbstractRawPacket::readStream(stream);
}


void ByeMsgPacket::writeStream(QDataStream *stream) const
{
  *stream << m_bye;
}
