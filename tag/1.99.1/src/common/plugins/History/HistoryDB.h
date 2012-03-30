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

#ifndef HISTORYDB_H_
#define HISTORYDB_H_

#include <QHash>
#include <QObject>

class MessageData;
class PrivateTab;

class HistoryDB : public QObject
{
  Q_OBJECT

public:
  HistoryDB(QObject *parent = 0);
  void open(const QByteArray &id, const QString &dir);

private slots:
  void clientStateChanged(int state);
  void synced(const QByteArray &channelId);

private:
  qint64 messageId(const QByteArray &id) const;
  qint64 userId(const QByteArray &id) const;
  void close();

  int m_lastMessages;
  QHash<QByteArray, qint64> m_cache;
  QString m_id;
};

#endif /* HISTORYDB_H_ */