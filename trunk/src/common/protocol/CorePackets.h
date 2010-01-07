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

  /// Коды ошибок.
  enum Error {
    ErrorNoError               = 0,   ///< Нет ошибки.
    ErrorNickAlreadyUse        = 200, ///< Выбранный ник уже занят.
    ErrorOldClientProtocol     = 100, ///< Клиент использует устаревшую версию протокола.
    ErrorOldServerProtocol     = 104, ///< Сервер использует устаревшую версию протокола.
    ErrorBadGreetingFlag       = 101, ///< Клиент отправил неподдерживаемый флаг приветствия.
    ErrorBadNickName           = 102, ///< Выбранный клиентом ник, не допустим в чате.
    ErrorBadUserAgent          = 103, ///< Клиент отправил недопустимый UserAgent.
    ErrorNotNetworkConfigured  = 400, ///< На сервере, к которому пытается слинковаться другой сервер, не настроена сеть.
    ErrorBadNetworkKey         = 401, ///< Ошибка проверки ключа сети.
    ErrorNumericAlreadyUse     = 403, ///< Ошибка линковки numeric подключаемого сервера уже используется.
    ErrorBadNumeric            = 404, ///< Ошибка линковки, неверный/некорректный numeric.
    ErrorRootServerIsSlave     = 405, ///< Ошибка подключения к корневому серверу, т.к он сконфигурирован как вторичный.
    ErrorUsersLimitExceeded    = 500, ///< Превышено максимально допустимое количество пользователей на этом сервере.
    ErrorLinksLimitExceeded    = 501, ///< Превышено максимально допустимое количество серверов подключенных к этому серверу.
    ErrorMaxUsersPerIpExceeded = 502  ///< Превышено максимально допустимое число пользователей с одного ip адреса.
  };

  HandshakeRequest();
  HandshakeRequest(AbstractProfile *profile, quint16 version = 3, Flag flag = FlagNone);
  inline AbstractProfile* profile() { return m_profile; }
  inline Error error() const        { return m_error; }
  inline quint8 flag() const        { return m_flag; }
  inline quint8 numeric() const     { return m_numeric; }
  static QList<quint16> opcodes()   { return QList<quint16>() << 100; }

protected:
  bool readStream(QDataStream *stream);
  Error verify();
  void writeStream(QDataStream *stream) const;

  quint16 m_version;          ///< версия протокола.
  quint8 m_flag;              ///< флаг Flag.
  quint8 m_gender;            ///< пол участника: 0 - мужской, 1 - женский (numeric сервера при FlagLink).
  QString m_nick;             ///< ник участника (рекомендуется пустая строка при FlagLink).
  QString m_name;             ///< полное имя участника, может быть пустой строкой (ключ сети при FlagLink).
  QString m_agent;            ///< строка идентифицирующая агент пользователя, формат: UserAgent/Version.
  QString m_bye;              ///< сообщение о выходе из чата.
  AbstractProfile *m_profile; ///< Профиль пользователя.
  Error m_error;              ///< Ошибка авторизации.
  quint8 m_numeric;           ///< Уникальный идентификатор сервера (при s2s соединении).
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
