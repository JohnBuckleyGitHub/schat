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

#ifndef CACHEDB_H_
#define CACHEDB_H_

#include <QObject>

#include "Channel.h"

class CacheDB : public QObject
{
  Q_OBJECT

public:
  CacheDB(QObject *parent = 0)
  : QObject(parent)
  {}

  inline static QString id() { return m_id; }
  static bool open(const QByteArray &id, const QString &dir);
  static ClientChannel channel(const QByteArray &id, bool feeds = true);
  static ClientChannel channel(qint64 id, bool feeds = true);
  static qint64 add(ClientChannel channel);
  static qint64 channelKey(const QByteArray &id, int type);
  static void close();
  static void saveData(Channel *channel);
  static void update(ClientChannel channel);

private:
  static QString m_id; ///< Идентификатор соединения с базой.
};

#endif /* CACHEDB_H_ */
