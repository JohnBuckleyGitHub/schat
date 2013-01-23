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
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "Storage.h"
#include "tools/OsInfo.h"

NodeServerFeed::NodeServerFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
  , m_date(0)
  , m_startupTime(0)
{
  init();
}


NodeServerFeed::NodeServerFeed(const QString &name, qint64 date)
  : Feed(name, date)
  , m_date(0)
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

  if (path == LS("uptime")) {
    FeedReply reply(Notice::OK);
    reply.json[LS("date")]    = m_startupTime;
    reply.json[LS("seconds")] = qAbs((DateTime::utc() - m_startupTime) / 1000);
    return reply;
  }

  return FeedReply(Notice::NotFound);
}


/*!
 * Получение тела фида.
 */
QVariantMap NodeServerFeed::feed(Channel *channel) const
{
  Channel *server = head().channel();
  if (server->type() != SimpleID::ServerId || !can(channel, Acl::Read))
    return QVariantMap();

  if (head().date() != m_date) {
    m_date = head().date();

    m_body.clear();
    m_body[LS("name")]    = server->name();
    m_body[LS("id")]      = SimpleID::encode(server->id());
    m_body[LS("version")] = QCoreApplication::applicationVersion();
    m_body[LS("os")]      = OsInfo::type();
    m_body[LS("users")]   = users();
    m_body[LS("auth")]    = auth();
  }

  return m_body;
}


QVariantMap NodeServerFeed::auth() const
{
  QVariantMap out;
  out[LS("anonymous")] = Storage::value(STORAGE_ANONYMOUS_AUTH).toBool();
  out[LS("external")]  = Storage::value(STORAGE_AUTH_SERVER).toString();
  return out;
}


/*!
 * Количество подключенных пользователей и пиковое число подключенных пользователей.
 */
QVariantMap NodeServerFeed::users() const
{
  QVariantMap out;
  out[LS("online")] = Ch::users().size();
  out[LS("peak")]   = Storage::value(STORAGE_PEAK_USERS).toMap();
  return out;
}


void NodeServerFeed::init()
{
  m_header.acl().setMask(0444);
  m_startupTime = DateTime::utc();
  head().data()[LS("date")] = m_startupTime;
}


Feed* NodeServerFeedCreator::create(const QString &name) const
{
  return new NodeServerFeed(name, DateTime::utc());
}


Feed* NodeServerFeedCreator::load(const QString &name, const QVariantMap &data) const
{
  return new NodeServerFeed(name, data);
}
