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

#ifndef COREPACKETS_H_
#define COREPACKETS_H_

#include <QByteArray>
#include <QList>
#include <QString>

/*!
 * \brief Абстрактный сырой пакет.
 */
class AbstractRawPacket
{
public:
  AbstractRawPacket(quint16 opcode = 0);
  virtual ~AbstractRawPacket();

  bool read(quint16 opcode, const QByteArray &body);
  inline bool isValid() const            { return m_valid; }
  inline quint16 opcode() const          { return m_opcode; }
  inline static QList<quint16> opcodes() { return QList<quint16>() << 0; }
  QByteArray data() const;

protected:
  virtual bool readStream(QDataStream *stream);
  virtual void writeStream(QDataStream *stream) const;

  bool m_valid;     ///< \a true в случае если пакет корректен.
  quint16 m_opcode; ///< Код пакета.
};


/*!
 * \brief Пакет, инициализирующий соединение.
 */
class HandshakeRequest : public AbstractRawPacket
{
public:
  enum Flag {
    FlagNone,
    FlagDirect,
    FlagLink
  };

  HandshakeRequest();
  HandshakeRequest(AbstractProfile *profile, quint16 version = 3, Flag flag = FlagNone);
  static QList<quint16> opcodes() { return QList<quint16>() << 100; }

protected:
  bool readStream(QDataStream *stream);
  void writeStream(QDataStream *stream) const;

  quint16 m_version; ///< версия протокола.
  quint8 m_flag;     ///< флаг Flag.
  quint8 m_gender;   ///< пол участника: 0 - мужской, 1 - женский (numeric сервера при FlagLink).
  QString m_nick;    ///< ник участника (рекомендуется пустая строка при FlagLink).
  QString m_name;    ///< полное имя участника, может быть пустой строкой (ключ сети при FlagLink).
  QString m_agent;   ///< строка идентифицирующая агент пользователя, формат: UserAgent/Version.
  QString m_bye;     ///< сообщение о выходе из чата.
};


/*!
 * \brief Отправка сообщения клиентом.
 */
class MessagePacket : public AbstractRawPacket
{
public:
  MessagePacket();
  MessagePacket(const QString &channel, const QString &message);
  inline QString channel() const           { return m_channel; }
  inline QString message() const           { return m_message; }
  static QList<quint16> opcodes()          { return QList<quint16>() << 200; }

protected:
  bool readStream(QDataStream *stream);
  void writeStream(QDataStream *stream) const;

  QString m_channel; ///< Канал (ник) для кого предназначено сообщение.
  QString m_message; ///< Сообщение.
};


/*!
 * \brief Отправка клиентом нового сообщения о выходе.
 */
class ByeMsgPacket : public AbstractRawPacket
{
public:
  ByeMsgPacket();
  ByeMsgPacket(const QString &bye);
  inline QString bye() const      { return m_bye; }
  static QList<quint16> opcodes() { return QList<quint16>() << 603; }

protected:
  bool readStream(QDataStream *stream);
  void writeStream(QDataStream *stream) const;

  QString m_bye; ///< Сообщение о выходе.
};

#endif /* COREPACKETS_H_ */
