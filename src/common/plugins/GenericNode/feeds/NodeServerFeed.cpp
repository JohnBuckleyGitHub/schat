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

#include <QCoreApplication>

#include "Ch.h"
#include "Channel.h"
#include "DateTime.h"
#include "feeds/NodeServerFeed.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "Storage.h"
#include "tools/OsInfo.h"

NodeServerFeed::NodeServerFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
  , m_startupTime(0)
{
  init();
}


NodeServerFeed::NodeServerFeed(const QString &name, qint64 date)
  : Feed(name, date)
  , m_startupTime(0)
{
  init();
}


Feed* NodeServerFeed::create(const QString &name)
{
  return new NodeServerFeed(name, DateTime::utc());
}


Feed* NodeServerFeed::load(const QString &name, const QVariantMap &data)
{
  return new NodeServerFeed(name, data);
}


/*!
 * Получение тела фида.
 */
QVariantMap NodeServerFeed::feed(Channel *channel)
{
  Channel *server = head().channel();
  if (server->type() != SimpleID::ServerId)
    return QVariantMap();

  QVariantMap header = head().get(channel);
  if (header.isEmpty())
    return QVariantMap();

  QVariantMap out;
  out[LS("head")]    = header;
  out[LS("name")]    = server->name();
  out[LS("id")]      = SimpleID::encode(server->id());
  out[LS("version")] = QCoreApplication::applicationVersion();
  out[LS("os")]      = OsInfo::type();

  QVariantMap users;
  users[LS("online")] = Ch::users().size();
  users[LS("peak")] = Storage::value(LS("PeakUsers")).toMap();

  out[LS("users")] = users;

  return out;
}


void NodeServerFeed::init()
{
  m_header.acl().setMask(0444);
  m_startupTime = DateTime::utc();
  head().data()[LS("date")] = m_startupTime;
}
