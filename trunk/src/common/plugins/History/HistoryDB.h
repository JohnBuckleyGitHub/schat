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

#ifndef HISTORYDB_H_
#define HISTORYDB_H_

#include <QHash>
#include <QObject>

#include "net/packets/MessageNotice.h"

class MessageId;

class HistoryDB : public QObject
{
  Q_OBJECT

  HistoryDB(QObject *parent = 0);

public:
  inline static QString id() { return m_id; }
  static bool open(const QByteArray &id, const QString &dir);
  static int status(int status);
  static QList<QByteArray> last(const QByteArray &channel, int limit);
  static MessageRecord get(const QByteArray &id);
  static void add(MessagePacket packet);
  static void clear();
  static void close();

private:
  static void create();
  static void version();

  static void V2();

  static QString m_id; ///< Идентификатор соединения с базой.
};

#endif /* HISTORYDB_H_ */
