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

#include "Ch.h"
#include "DateTime.h"
#include "feeds/FeedsCore.h"
#include "feeds/NodeListFeed.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "NodeChannelIndex.h"
#include "NodeChannelsPlugin_p.h"
#include "sglobal.h"

NodeListFeed::NodeListFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
  init();
}


NodeListFeed::NodeListFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  init();
}


/*!
 * Переопределение запроса \b delete.
 */
FeedReply NodeListFeed::del(const QString &path, Channel *channel)
{
  Q_UNUSED(path)
  Q_UNUSED(channel)
  return Notice::Forbidden;
}


/*!
 * Переопределение запроса \b post.
 */
FeedReply NodeListFeed::post(const QString &path, const QVariantMap &json, Channel *channel)
{
  Q_UNUSED(path)
  Q_UNUSED(json)
  Q_UNUSED(channel)
  return Notice::Forbidden;
}


/*!
 * Переопределение запроса \b put.
 */
FeedReply NodeListFeed::put(const QString &path, const QVariantMap &json, Channel *channel)
{
  Q_UNUSED(path)
  Q_UNUSED(json)
  Q_UNUSED(channel)

  if (path == LS("channels") && Ch::server() == channel) {
    const QList<ChannelIndexData> &list = NodeChannelsImpl::index()->list();
    QVariantList channels;

    foreach (const ChannelIndexData &data, list)
      channels.push_back(data.toList());

    m_data[LS("channels")] = channels;
    m_header.setDate(NodeChannelsImpl::index()->date());
    return FeedReply(Notice::OK, DateTime::utc());
  }

  return Notice::Forbidden;
}


void NodeListFeed::init()
{
  m_header.acl().setMask(0444);
  m_data[LS("format")] = QVariantList() << LS("id") << LS("name") << LS("count") << LS("title") << LS("options");

  FeedsCore::sub(FEED_NAME_LIST);
}


Feed* NodeListFeedCreator::create(const QString &name) const
{
  return new NodeListFeed(name, DateTime::utc());
}


Feed* NodeListFeedCreator::load(const QString &name, const QVariantMap &data) const
{
  return new NodeListFeed(name, data);
}
