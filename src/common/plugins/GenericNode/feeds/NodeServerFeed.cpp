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

#include <QCoreApplication>

#include "Ch.h"
#include "Channel.h"
#include "DateTime.h"
#include "feeds/NodeServerFeed.h"
#include "feeds/ServerFeed.h"
#include "net/packets/Notice.h"
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


/*!
 * Обработка \b get запросов.
 *
 * Поддерживаемые запросы:
 * - \b uptime информация о дате запуска сервера и количестве секунд с момента запуска.
 */
FeedReply NodeServerFeed::get(const QString &path, const QVariantMap &json, Channel *channel) const
{
  Q_UNUSED(json)
  Q_UNUSED(channel)

  if (path == SERVER_FEED_UPTIME_KEY) {
    FeedReply reply(Notice::OK);
    reply.json[SERVER_FEED_DATE_KEY]    = m_startupTime;
    reply.json[SERVER_FEED_SECONDS_KEY] = qAbs((DateTime::utc() - m_startupTime) / 1000);
    return reply;
  }

  return FeedReply(Notice::NotFound);
}


void NodeServerFeed::setChannel(Channel *channel)
{
  Feed::setChannel(channel);

  m_data[SERVER_FEED_NAME_KEY] = channel->name();
  m_data[SERVER_FEED_ID_KEY]   = SimpleID::encode(channel->id());
}


QVariantMap NodeServerFeed::auth() const
{
  QVariantMap out;
  out[LS("anonymous")] = Storage::value(STORAGE_ANONYMOUS_AUTH).toBool();
  out[LS("external")]  = Storage::value(STORAGE_AUTH_SERVER).toString();
  return out;
}


void NodeServerFeed::init()
{
  m_startupTime = DateTime::utc();
  m_header.acl().setMask(0444);
  m_header.setDate(m_startupTime);

  m_data[SERVER_FEED_VERSION_KEY] = QCoreApplication::applicationVersion();
  m_data[SERVER_FEED_OS_KEY]      = OsInfo::type();
  m_data[SERVER_FEED_AUTH_KEY]    = auth();
}


Feed* NodeServerFeedCreator::create(const QString &name) const
{
  return new NodeServerFeed(name, DateTime::utc());
}


Feed* NodeServerFeedCreator::load(const QString &name, const QVariantMap &data) const
{
  return new NodeServerFeed(name, data);
}
