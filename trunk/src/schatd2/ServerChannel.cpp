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

#include "Account.h"
#include "Ch.h"
#include "DataBase.h"
#include "DateTime.h"
#include "feeds/FeedsCore.h"
#include "feeds/FeedStorage.h"
#include "feeds/FeedStrings.h"
#include "net/SimpleID.h"
#include "Normalize.h"
#include "ServerChannel.h"
#include "sglobal.h"
#include "User.h"

ServerChannel::ServerChannel(ClientChannel channel)
  : Channel(channel->id(), channel->name())
  , m_permanent(false)
  , m_hosts(0)
  , m_user(0)
{
  normalize();
  m_channels.set(channel->channels().all());

  if (type() == SimpleID::UserId) {
    m_hosts = new Hosts(this);
    m_user  = new User();
  }
}


ServerChannel::ServerChannel(const QByteArray &id, const QString &name)
  : Channel(id, name)
  , m_permanent(false)
  , m_hosts(0)
  , m_user(0)
{
  normalize();

  if (type() == SimpleID::UserId) {
    m_hosts = new Hosts(this);
    m_user  = new User();
  }
}


ServerChannel::~ServerChannel()
{
  if (m_hosts)
    delete m_hosts;

  if (m_user)
    delete m_user;
}


/*!
 * Добавление канала в список каналов.
 */
bool ServerChannel::addChannel(const QByteArray &id)
{
  if (m_channels.add(id)) {
    m_offline.remove(id);

    if (SimpleID::typeOf(id) == SimpleID::UserId)
      FeedsCore::post(this, FEED_NAME_USERS + LC('/') + SimpleID::encode(id), Ch::server().data(), QVariant(), Feed::Broadcast);

    return true;
  }

  return false;
}


/*!
 * Удаление канала из списка каналов.
 */
bool ServerChannel::removeChannel(const QByteArray &id, bool offline)
{
  if (m_channels.contains(id)) {
    m_channels.remove(id);

    if (offline)
      m_offline.add(id);

    if (SimpleID::typeOf(id) == SimpleID::UserId)
      FeedsCore::del(this, FEED_NAME_USERS + LC('/') + SimpleID::encode(id), Ch::server().data(), Feed::Broadcast);

    return true;
  }

  return false;
}


/*!
 * Установка имени канала.
 */
bool ServerChannel::setName(const QString &name)
{
  if (Channel::setName(name)) {
    normalize();
    return true;
  }

  return false;
}


void ServerChannel::createAccount()
{
  m_account->date    = DateTime::utc();
  m_account->channel = key();
  m_account->setDate(DateTime::utc());

  DataBase::add(account());
}


void ServerChannel::setKey(qint64 key)
{
  Channel::setKey(key);

  if (m_user) {
    m_user->channel = key;

    if (!m_user->saved)
      DataBase::add(m_user);
  }
}


bool ServerChannel::canEdit(ChatChannel channel, bool special)
{
  return feed(FEED_NAME_ACL)->can(channel.data(), Acl::Edit | (special ? Acl::SpecialEdit : 0));
}


bool ServerChannel::canRead(ChatChannel channel, bool special)
{
  return feed(FEED_NAME_ACL)->can(channel.data(), Acl::Read | (special ? Acl::SpecialRead : 0));
}


bool ServerChannel::canWrite(ChatChannel channel, bool special)
{
  return feed(FEED_NAME_ACL)->can(channel.data(), Acl::Write | (special ? Acl::SpecialWrite : 0));
}


void ServerChannel::normalize()
{
  m_normalized = Normalize::toId(this);
}
