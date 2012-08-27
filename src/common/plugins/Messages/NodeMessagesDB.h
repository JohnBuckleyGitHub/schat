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

#include <QObject>
#include <QRunnable>

#include "net/packets/MessageNotice.h"
#include "text/MessageId.h"

class MessageNotice;
class QSqlQuery;

class NodeMessagesDB : public QObject
{
  Q_OBJECT

public:
  NodeMessagesDB(QObject *parent = 0);
  inline static QString id() { return m_id;}
  static bool open();
  static int status(int status);
  static QList<MessageId> ids(QSqlQuery &query);
  static QList<MessageId> last(const QByteArray &channel, int limit);
  static QList<MessageId> last(const QByteArray &user1, const QByteArray &user2, int limit);
  static QList<MessageRecord> get(const QList<MessageId> &ids);
  static QList<MessageRecord> messages(QSqlQuery &query);
  static QList<MessageRecord> offline(const QByteArray &user);
  static void add(const MessageNotice &packet, int status = 300);
  static void markAsRead(const QList<MessageRecord> &records);

private slots:
  void startTasks();

private:
  static void version();

  static void V2();

  QList<QRunnable*> m_tasks;     ///< Задачи для выполнения в отдельном потоке.
  static bool m_isOpen;          ///< true если база открыта.
  static NodeMessagesDB *m_self; ///< Указатель на себя.
  static QString m_id;           ///< Идентификатор сооединения с базой, это строка всегда равна "messages".
};


class AddMessageTask : public QRunnable
{
public:
  /// Формат данных в таблице storage.
  enum Format {
    AutoFormat,    ///< Автоматическое определение формата.
    JSonFormat,    ///< JSON формат.
  };

  AddMessageTask(const MessageNotice &packet, int status = 300);
  void run();

private:
  int m_status;           ///< Статус сообщения.
  MessageNotice m_packet; ///< Копия пакета.
};

#endif /* NODEMESSAGESDB_H_ */
