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

#include "net/PacketWriter.h"
#include "schat.h"

class PacketReader;
class MessageData;

class SCHAT_EXPORT NoticeData
{
public:
  /// Тип уведомления.
  enum Type {
    Invalid,          ///< Некорректный тип.
    MessageDelivered, ///< Сообщение было доставлено.
    MessageRejected,  ///< Сообщение было отклонено.
    SlaveNodeXHost    ///< Оригинальный адрес пользователя.
  };

  enum Param1 {
    UnknownError,    ///< Неизвестная ошибка.
    UserUnavailable, ///< Пользователь недоступен.
    Ignored,         ///< Сообщение было игнорированно.
  };

  NoticeData()
  : timestamp(0)
  , type(0)
  , messageName(0)
  , param1(0)
  , param2(0)
  {}

  NoticeData(quint16 type, const QByteArray &senderId, const QByteArray &destId, const QString &text);
  NoticeData(const QByteArray &senderId, const QByteArray &destId, quint16 type, quint64 messageName, quint8 param1 = 0);

  QByteArray destId;   ///< Идентификатор назначения.
  QByteArray senderId; ///< Идентификатор отправителя.
  qint64 timestamp;    ///< Отметка времени.
  quint16 type;        ///< Тип.
  quint64 messageName; ///< Имя-счётчик сообщения, только для типов MessageDelivered и MessageRejected.
  quint8 param1;       ///< Не обязательный параметр №1
  quint8 param2;       ///< Не обязательный параметр №2
  QString text;        ///< Текстовое содержимое.
};


/*!
 * Абстрактное уведомление.
 */
class AbstractNotice
{
public:
  enum Type {
    MessageNoticeType = 0x6D ///< 'm'.
  };

  AbstractNotice(quint16 type, const QByteArray &sender, const QByteArray &dest = QByteArray())
  : m_sender(sender)
  , m_dest(QList<QByteArray>() << dest)
  , m_type(type)
  {}

  AbstractNotice(quint16 type, const QByteArray &sender, const QList<QByteArray> &dest = QList<QByteArray>())
  : m_sender(sender)
  , m_dest(dest)
  , m_type(type)
  {}

  inline int type() const { return m_type; }
  inline QByteArray dest() const    { if (m_dest.size()) return m_dest.at(0); else return QByteArray(); }
  inline QByteArray sender() const  { return m_sender; }
  inline QList<QByteArray> destinations() const { return m_dest; }

protected:
  QByteArray m_sender;
  QList<QByteArray> m_dest;
  quint16 m_type;
};


/*!
 * Пакет Protocol::NoticePacket с типом MessageNoticeType.
 */
class SCHAT_EXPORT MessageNotice : public AbstractNotice
{
public:
  enum Status {
    Delivered, ///< Сообщение было доставлено.
    Rejected,  ///< Сообщение было отклонено.
  };

  enum Error {
    NoError,         ///< Нет ошибки.
    UnknownError,    ///< Неизвестная ошибка.
    UserUnavailable, ///< Пользователь недоступен.
    Ignored,         ///< Сообщение было игнорированно.
  };

  MessageNotice(quint16 type, PacketReader *reader);
  MessageNotice(quint8 status, const QByteArray &sender, const QByteArray &dest, const QByteArray &id, quint8 error = NoError);
  MessageNotice(quint8 status, MessageData *data, quint8 error = NoError);
  inline int error() const { return m_error; }
  inline int fields() const { return m_fields; }
  inline int status() const { return m_status; }
  inline QByteArray id() const { return m_id; }
  QByteArray data(QDataStream *stream) const;

private:
  bool m_valid;    ///< true если данные корректны.
  quint8 m_fields; ///< Дополнительные поля данных.
  quint8 m_status; ///< Тип, \sa Status.
  quint8 m_error;  ///< Код ошибки, \sa Error.
  QByteArray m_id; ///< Идентификатор сообщения.
};


/*!
 * Формирует пакет Protocol::NoticePacket.
 */
class SCHAT_EXPORT NoticeWriter : public PacketWriter
{
public:
  NoticeWriter(QDataStream *stream, const NoticeData &data);
};


/*!
 * Читает пакет Protocol::NoticePacket.
 */
class SCHAT_EXPORT NoticeReader
{
public:
  NoticeReader(PacketReader *reader);

  NoticeData data;
};

#endif /* NOTICES_H_ */
