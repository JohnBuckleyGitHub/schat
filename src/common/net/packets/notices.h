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

#include <QVariant>

#include "schat.h"

class PacketReader;
class PacketWriter;

/*!
 * Универсальное уведомление, содержит данные в JSON формате, текстовый тип, отметку времени и уникальный идентификатор.
 * Этот пакет является универсальным высокоуровневым транспортом чата, для расширений протокола.
 */
class SCHAT_EXPORT Notice
{
public:
  enum Type {
    GenericType = 103,  ///< 'g'
    ChannelType = 99    ///< 'c'
  };

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
    UserAlreadyExists   = 404, ///< User Already Exists.
    UserNotExists       = 405, ///< User Not Exists.
    NickAlreadyUse      = 406, ///< Nick Already In Use.
    UserOffline         = 407, ///< User Offline.
    Conflict            = 408, ///< Conflict.
    InternalError       = 500, ///< Internal Error.
    NotImplemented      = 501, ///< Not Implemented.
    BadGateway          = 502, ///< Bad Gateway.
    ServiceUnavailable  = 503, ///< Service Unavailable.
    GatewayTimeout      = 504  ///< Gateway Timeout.
  };

  Notice(const QByteArray &sender, const QByteArray &dest, const QString &command, quint64 time = 0, const QByteArray &id = QByteArray(), const QVariantMap &data = QVariantMap());
  Notice(const QByteArray &sender, const QList<QByteArray> &dest, const QString &command, quint64 time = 0, const QByteArray &id = QByteArray(), const QVariantMap &data = QVariantMap());
  Notice(quint16 type, PacketReader *reader);
  virtual ~Notice() {}

  virtual bool isValid() const;

  inline const QByteArray& id() const      { return m_id; }
  inline const QByteArray& raw() const     { return m_raw; }
  inline const QByteArray& sender() const  { return m_sender; }
  inline const QList<QByteArray>& destinations() const { return m_dest; }
  inline const QString& command() const    { return m_command; }
  inline const QString& text() const       { return m_text; }
  inline const QVariantMap& json() const   { return m_data; }
  inline int fields() const                { return m_fields; }
  inline int status() const                { return m_status; }
  inline int type() const                  { return m_type; }
  inline int version() const               { return m_version; }
  inline QByteArray dest() const           { if (m_dest.size()) return m_dest.at(0); else return QByteArray(); }
  inline qint64 time() const               { return m_time; }
  QByteArray data(QDataStream *stream, bool echo = false) const;

  static QString status(int status);

  inline void setDest(const QByteArray &dest) { m_dest = QList<QByteArray>() << dest; }
  inline void setDest(const QList<QByteArray> &dest) { m_dest = dest; }
  inline void setStatus(int status) { m_status = status; }
  void setText(const QString &text);

protected:
  virtual void write(PacketWriter *writer) const { Q_UNUSED(writer) }

  QByteArray m_sender;      ///< Идентификатор отправителя.
  QList<QByteArray> m_dest; ///< Идентификаторы получателей.
  quint16 m_type;           ///< Тип пакета Notice::Type.
  quint8 m_fields;          ///< Дополнительные поля данных.
  quint8 m_version;         ///< Версия пакета, обязательное поле.
  quint16 m_status;         ///< Статус \sa StatusCodes, обязательное поле.
  qint64 m_time;            ///< Отметка времени, обязательное поле.
  QByteArray m_id;          ///< Идентификатор сообщения, не обязательное поле.
  QString m_command;        ///< Текстовая команда, обязательное поле.
  QVariantMap m_data;       ///< JSON данные пакета, не обязательное поле.
  QByteArray m_raw;         ///< Сырые JSON данные.
  QString m_text;           ///< Сырой текст, не обязательное поле.
};

#endif /* NOTICES_H_ */
