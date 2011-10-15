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

#ifndef NOTICES_H_
#define NOTICES_H_

#include "net/PacketReader.h"
#include "net/PacketWriter.h"
#include "schat.h"

class PacketReader;
class MessageData;

/*!
 * Абстрактное уведомление.
 */
class AbstractNotice
{
public:
  enum Type {
    MessageNoticeType = 0x6D, ///< 'm'.
    TextNoticeType = 0x74,    ///< 't'
    GenericNoticeType = 0x67  ///< 'g'
  };

  AbstractNotice(quint16 type, PacketReader *reader)
  : m_sender(reader->sender())
  , m_dest(reader->destinations())
  , m_type(type)
  , m_fields(0)
  {
    m_fields = reader->get<quint8>();
  }

  AbstractNotice(quint16 type, const QByteArray &sender, const QByteArray &dest = QByteArray())
  : m_sender(sender)
  , m_dest(QList<QByteArray>() << dest)
  , m_type(type)
  , m_fields(0)
  {}

  AbstractNotice(quint16 type, const QByteArray &sender, const QList<QByteArray> &dest = QList<QByteArray>())
  : m_sender(sender)
  , m_dest(dest)
  , m_type(type)
  , m_fields(0)
  {}

  inline int fields() const         { return m_fields; }
  inline int type() const           { return m_type; }
  inline QByteArray dest() const    { if (m_dest.size()) return m_dest.at(0); else return QByteArray(); }
  inline QByteArray sender() const  { return m_sender; }
  inline QList<QByteArray> destinations() const { return m_dest; }
  inline void setDest(const QByteArray &dest) { m_dest = QList<QByteArray>() << dest; }
  inline void setDest(const QList<QByteArray> &dest) { m_dest = dest; }

protected:
  QByteArray m_sender;
  QList<QByteArray> m_dest;
  quint16 m_type;
  quint8 m_fields; ///< Дополнительные поля данных.
};


/*!
 * Пакет Protocol::NoticePacket с типом MessageNoticeType.
 * \deprecated Заменить этот класс на Notice.
 */
class SCHAT_EXPORT MessageNotice : public AbstractNotice
{
public:
  enum Status {
    Delivered = 0x64, ///< 'd' Сообщение было доставлено.
    Rejected = 0x72   ///< 'r' Сообщение было отклонено.
  };

  enum Error {
    NoError = 0,            ///< Нет ошибки.
    UnknownError = 0x75,    ///< 'u' Неизвестная ошибка.
    UserUnavailable = 0x55, ///< 'U' Пользователь недоступен.
    Ignored = 0x69          ///< 'i' Сообщение было игнорированно.
  };

  MessageNotice(quint16 type, PacketReader *reader);
  MessageNotice(quint8 status, const QByteArray &sender, const QByteArray &dest, const QByteArray &id, quint8 error = NoError);
  inline int error() const { return m_error; }
  inline int status() const { return m_status; }
  inline QByteArray id() const { return m_id; }
  QByteArray data(QDataStream *stream) const;

private:
  bool m_valid;    ///< true если данные корректны.
  quint8 m_status; ///< Тип, \sa Status.
  quint8 m_error;  ///< Код ошибки, \sa Error.
  QByteArray m_id; ///< Идентификатор сообщения.
};


/*!
 * \deprecated Заменить этот класс на Notice.
 */
class SCHAT_EXPORT TextNotice : public AbstractNotice
{
public:
  enum SubType {
    SlaveNodeXHost = 0x5848 ///< 'XH'
  };

  TextNotice(quint16 type, PacketReader *reader);
  TextNotice(quint16 sybtype, const QByteArray &sender, const QByteArray &dest, const QString &text);
  inline int subtype() const { return m_subtype; }
  inline QString text() const { return m_text; }
  inline void setText(const QString &text) { m_text = text; }
  QByteArray data(QDataStream *stream) const;

private:
  quint16 m_subtype; ///< Тип.
  QString m_text;    ///< Текст.
};


/*!
 * Универсальное уведомление, содержит данные в JSON формате, текстовый тип, отметку времени и уникальный идентификатор.
 * Этот пакет является универсальным высокоуровневым транспортом чата, для расширений протокола.
 */
class SCHAT_EXPORT Notice : public AbstractNotice
{
public:
  /// Дополнительные поля данных.
  enum Fields {
    NoFields = 0,  ///< Нет дополнительных полей.
    IdField = 1,   ///< Содержит идентификатор сообщения \sa m_id.
    JSonField = 2, ///< Содержит JSON данные \sa m_data, m_raw.
    TextField = 4  ///< Содержит сырые текстовые данные \sa m_text.
  };

  ///< Коды состояния.
  enum StatusCodes {
    OK                  = 200, ///< OK.
    BadRequest          = 400, ///< Bad Request.
    Unauthorized        = 401, ///< Unauthorized.
    Forbidden           = 402, ///< Forbidden.
    NotFound            = 403, ///< Not Found.
    InternalError       = 500, ///< Internal Error.
    NotImplemented      = 501, ///< Not Implemented.
    BadGateway          = 502, ///< Bad Gateway.
    ServiceUnavailable  = 503, ///< Service Unavailable.
    GatewayTimeout      = 504, ///< Gateway Timeout.
    VersionNotSupported = 505, ///< Version Not Supported.
    UserAlreadyExists   = 506, ///< User Already Exists.
    UserNotExists       = 507, ///< User Not Exists.
    NickAlreadyUse      = 508  ///< Nick Already In Use.
  };

  Notice(quint16 type, PacketReader *reader);
  Notice(const QByteArray &sender, const QByteArray &dest, const QString &command, const QVariant &data = QVariant(), quint64 time = 0, const QByteArray &id = QByteArray());
  bool isValid() const;
  inline int status() const { return m_status; }
  inline int version() const { return m_version; }
  inline QByteArray id() const { return m_id; }
  inline QByteArray raw() const { return m_raw; }
  inline qint64 time() const { return m_time; }
  inline QString command() const { return m_command; }
  inline QString text() const { return m_text; }
  inline void setStatus(int status) { m_status = status; }
  QByteArray data(QDataStream *stream, bool echo = false) const;
  QVariant json() const;
  static QString status(int status);
  void setText(const QString &text);

private:
  quint8 m_version;  ///< Версия пакета, обязательное поле.
  quint16 m_status;  ///< Статус \sa StatusCodes, обязательное поле.
  qint64 m_time;     ///< Отметка времени, обязательное поле.
  QByteArray m_id;   ///< Идентификатор сообщения, не обязательное поле.
  QString m_command; ///< Текстовая команда, обязательное поле.
  QVariant m_data;   ///< JSON данные пакета, не обязательное поле.
  QByteArray m_raw;  ///< Сырые данные, поле используется только при чтении пакета, для того чтобы не запускать разбор JSON без необходимости.
  QString m_text;    ///< Сырой текст, не обязательное поле.
};

#endif /* NOTICES_H_ */
