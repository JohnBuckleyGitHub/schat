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

#include "debugstream.h"

#include "Channel.h"
#include "net/SimpleID.h"
#include "text/HtmlFilter.h"

Channel::Channel()
  : m_synced(false)
  , m_valid(true)
{
}


Channel::Channel(const QByteArray &id, const QString &name)
  : m_synced(false)
  , m_valid(true)
{
  setId(id);
  setName(name);
}


Channel::~Channel()
{
}


bool Channel::addUser(const QByteArray &id)
{
  if (id.size() != SimpleID::DefaultSize)
    return false;

  if (m_users.contains(id))
    return false;

  SCHAT_DEBUG_STREAM(this << "Channel::addUser()" << SimpleID::encode(id))
  m_users.append(id);
  return true;
}


bool Channel::removeUser(const QByteArray &id)
{
  if (id.size() != SimpleID::DefaultSize)
    return false;

  if (!m_users.contains(id))
    return false;

  SCHAT_DEBUG_STREAM(this << "Channel::removeUser()" << SimpleID::encode(id))
  m_users.removeAll(id);
  return true;
}


bool Channel::setId(const QByteArray &id)
{
  m_id = id;
  return validate(id.size() == SimpleID::DefaultSize);
}


bool Channel::setName(const QString &name)
{
  if (name.isEmpty())
    return validate(false);

  if (name.size() > (MaxNameLength * 2))
    return validate(false);

  m_name = name.simplified().left(MaxNameLength);
  if (name.size() < MinNameLengh)
    return validate(false);

  return validate(true);
}


bool Channel::setTopic(const QString &topic)
{
  HtmlFilter filter(0, MaxTopicLength, 0);
  m_topic = filter.filter(topic);

  return validate(true);
}


bool Channel::setTopic(const QString &topic, const QByteArray &authorId, qint64 timestamp)
{
  if (SimpleID::typeOf(authorId) != SimpleID::UserId || timestamp == 0)
    return false;

  QVariantMap map = m_data["topic"].toMap();
  map["author"] = QString(SimpleID::encode(authorId));
  map["timestamp"] = QString::number(timestamp);
  m_data["topic"] = map;

  return setTopic(topic);
}


bool Channel::setUsers(const QList<QByteArray> &users)
{
  if (users.size() == 0)
    return validate(false);

  m_users = users;
  return validate(true);
}


Topic Channel::topic() const
{
  Topic topic;
  topic.channel = m_id;
  topic.author = SimpleID::decode(m_data["topic"].toMap()["author"].toByteArray());
  topic.timestamp = m_data["topic"].toMap()["timestamp"].toLongLong();
  topic.topic = m_topic;
  return topic;
}


/*!
 * Добавления фида, если фид с этим именем уже существует, то он будет заменён.
 *
 * \param feed Указатель на фид.
 * \return \b false если фид не корректен и не был добавлен.
 */
bool Channel::addFeed(FeedPtr feed)
{
  if (!feed)
    return false;

  feed->h().setId(m_id);

  if (!feed->isValid())
    return false;

  m_feeds[feed->h().name()] = feed;
  return true;
}
