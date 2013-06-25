/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#ifndef MESSAGENOTICE_H_
#define MESSAGENOTICE_H_

#include <QStringList>

#include "id/ChatId.h"
#include "net/packets/Notice.h"

struct SCHAT_EXPORT MessageRecord
{
  MessageRecord()
  : id(0)
  , status(200)
  , date(0)
  {}

  qint64 id;
  QByteArray messageId;
  QByteArray senderId;
  QByteArray destId;
  qint64 status;
  qint64 date;
  QString command;
  QString text;
  QString plain;
  QByteArray data;
};


class SCHAT_EXPORT MessageNotice : public Notice
{
public:
  MessageNotice();
  MessageNotice(const MessageRecord &record, bool parse = false);
  MessageNotice(const QByteArray &sender, const QByteArray &dest, const QString &text, quint64 date = 0, const QByteArray &id = QByteArray());
  MessageNotice(quint16 type, PacketReader *reader);
  inline const QByteArray& internalId() const     { return m_internalId; }
  inline void setInternalId(const QByteArray &id) { m_internalId = id; }
  QByteArray toId() const;
  static QList<QByteArray> decode(const QStringList &ids);
  static QStringList encode(const QList<QByteArray> &ids);
  static QByteArray toTag(const QStringList &messages);

  ObjectId oid;

private:
  QByteArray m_internalId; ///< Внутренний идентификатор сообщения.
};

typedef QSharedPointer<MessageNotice> MessagePacket;

#endif /* MESSAGENOTICE_H_ */
