/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include "feeds/Feed.h"
#include "net/packets/MessageNotice.h"

class MessageId;
class QRunnable;

class HistoryDB : public QObject
{
  Q_OBJECT

public:
  HistoryDB(QObject *parent = 0);

  inline static QString id() { return m_id; }
  static bool open(const QByteArray &id, const QString &dir);
  static bool synced(FeedPtr feed);
  static int status(int status);
  static MessageRecord get(const QByteArray &id);
  static QList<QByteArray> last(const QByteArray &channel, int limit);
  static void add(MessagePacket packet);
  static void clear();
  static void close();

private slots:
  void startTasks();

private:
  static void create();
  static void version();

  static qint64 V2();
  static qint64 V3();

  QList<QRunnable*> m_tasks; ///< Задачи для выполнения в отдельном потоке.
  static HistoryDB *m_self;  ///< Указатель на себя.
  static QString m_id;       ///< Идентификатор соединения с базой.
};

#endif /* HISTORYDB_H_ */
