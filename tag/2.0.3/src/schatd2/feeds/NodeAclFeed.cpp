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

#include "acl/AclValue.h"
#include "Ch.h"
#include "Channel.h"
#include "DateTime.h"
#include "feeds/FeedsCore.h"
#include "feeds/NodeAclFeed.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "sglobal.h"

NodeAclFeed::NodeAclFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
  init();
}


NodeAclFeed::NodeAclFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  init();
}


/*!
 * Переопределение запросов "delete acl/owner/<id>" и "delete acl/other/<id>".
 */
FeedReply NodeAclFeed::del(const QString &path, Channel *channel)
{
  if (!path.startsWith(LS("head/")))
    return Notice::Forbidden;

  const int acl = AclValue::match(this, channel);
  FeedReply reply(Notice::Forbidden);

  if (acl & (Acl::Edit | Acl::SpecialEdit)) {
    reply = Feed::del(path, channel);
  }
  else if (acl & Acl::SpecialWrite) {
    const QByteArray id = SimpleID::decode(path.mid(11));
    if (SimpleID::typeOf(id) != SimpleID::UserId)
      return Notice::BadRequest;

    if (isGenericUser(id)) {
      reply.status = head().del(path.mid(5));
      if (reply.status != Notice::OK)
        return reply;
    }
  }

  if (reply.status == Notice::OK) {
    m_data.remove(path.mid(11));
    reply.date = DateTime::utc();
  }

  return reply;
}


/*!
 * Переопределение запросов "post acl/head/owner" или "post acl/head/other/<id>".
 */
FeedReply NodeAclFeed::post(const QString &path, const QVariantMap &json, Channel *channel)
{
  if (!path.startsWith(LS("head/")))
    return Notice::Forbidden;

  const int acl = AclValue::match(this, channel);
  FeedReply reply(Notice::Forbidden);

  if (acl & (Acl::Edit | Acl::SpecialEdit) || path == LS("head/owner")) {
    reply = Feed::post(path, json, channel);
  }
  else if (acl & Acl::SpecialWrite && path.startsWith(LS("head/other/"))) {
    const QByteArray id = SimpleID::decode(path.mid(11));
    if (SimpleID::typeOf(id) != SimpleID::UserId)
      return Notice::BadRequest;

    if (isGenericUser(id)) {
      reply.status = head().post(path.mid(5), json[FEED_KEY_VALUE]);
      if (reply.status != Notice::OK)
        return reply;
    }
  }

  if (reply.status == Notice::OK) {
    if (path == LS("head/owner"))
      m_data[json.value(FEED_KEY_VALUE).toString()] = AclValue::toByteArray(head().acl().mask() >> 6 | Acl::SpecialEdit);
    else
      m_data[path.mid(11)] = AclValue::toByteArray(json.value(FEED_KEY_VALUE).toInt());

    reply.date = DateTime::utc();
  }

  return reply;
}


/*!
 * Переопределение запроса "put acl/head/mask" для установки прав доступа для пользователей
 * права которых не установлены явно.
 */
FeedReply NodeAclFeed::put(const QString &path, const QVariantMap &json, Channel *channel)
{
  if (path.startsWith(LS("head/"))) {
    const FeedReply reply = Feed::put(path, json, channel);
    if (reply.status == Notice::OK)
      m_data[FEED_WILDCARD_ASTERISK] = AclValue::toByteArray(head().acl().mask() & ~0770);

    return reply;
  }

  return Notice::Forbidden;
}


/*!
 * Переопределение установки канала для записи информации о владельце канала.
 */
void NodeAclFeed::setChannel(Channel *channel)
{
  Feed::setChannel(channel);

  if (!channel)
    return;

  if (channel->type() == SimpleID::UserId)
    m_data[SimpleID::encode(channel->id())] = AclValue::toByteArray(head().acl().mask() >> 6 | Acl::SpecialEdit);

  if (channel->type() == SimpleID::ServerId)
    FeedsCore::sub(FEED_NAME_ACL);
}


/*!
 * Возвращает \b true если пользователь не является владельцем или модератором.
 */
bool NodeAclFeed::isGenericUser(const QByteArray &id) const
{
  ChatChannel user = Ch::channel(id, SimpleID::UserId);
  if (!user)
    return true;

  const int acl = AclValue::match(this, user.data());
  Ch::gc(user);

  if (acl & (Acl::Edit | Acl::SpecialEdit))
    return false;

  if (acl & (Acl::SpecialWrite))
    return false;

  return true;
}


/*!
 * Инициализация фида.
 */
void NodeAclFeed::init()
{
  if (!m_data.isEmpty())
    return;

  m_data[FEED_WILDCARD_ASTERISK] = AclValue::toByteArray(head().acl().mask() & ~0770);

  // Импортирование существующих прав доступа.
  const QVariantMap &data = head().data();
  const QVariantMap others = data.value(ACL_OTHERS).toMap();
  if (!others.isEmpty()) {
    QMapIterator<QString, QVariant> i(others);
    while (i.hasNext()) {
      i.next();
      m_data[i.key()] = AclValue::toByteArray(i.value().toInt());
    }
  }

  const QStringList owners = data.value(ACL_OWNERS).toStringList();
  if (!owners.isEmpty()) {
    const QString mask = AclValue::toByteArray(head().acl().mask() >> 6 | Acl::SpecialEdit);
    foreach (const QString &id, owners)
      m_data[id] = mask;
  }
}


Feed* NodeAclFeedCreator::create(const QString &name) const
{
  return new NodeAclFeed(name, DateTime::utc());
}


Feed* NodeAclFeedCreator::load(const QString &name, const QVariantMap &data) const
{
  return new NodeAclFeed(name, data);
}
