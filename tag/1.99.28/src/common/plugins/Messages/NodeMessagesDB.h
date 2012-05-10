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

#ifndef NODEMESSAGESDB_H_
#define NODEMESSAGESDB_H_

#include <QString>

#include "text/MessageId.h"

class MessageNotice;
class QSqlQuery;

class NodeMessagesDB
{
  NodeMessagesDB();

public:
  static bool open();
  static int status(int status);
  static QList<MessageId> ids(QSqlQuery &query);
  static QList<MessageId> last(const QByteArray &channel, int limit);
  static QList<MessageId> last(const QByteArray &user1, const QByteArray &user2, int limit);
  static QVariantList get(const QList<MessageId> &ids);
  static QVariantList messages(QSqlQuery &query);
  static QVariantList offline(const QByteArray &user);
  static void add(const MessageNotice &packet, int status = 300);
  static void markAsRead(const QVariantList &data);

private:
  static void version();

  static bool m_isOpen; ///< true если база открыта.
  static QString m_id;  ///< Идентификатор сооединения с базой, это строка всегда равна "messages".
};

#endif /* NODEMESSAGESDB_H_ */
