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

#include "Account.h"
#include "DateTime.h"
#include "feeds/AccountFeed.h"
#include "feeds/FeedStorage.h"
#include "net/packets/auth.h"
#include "net/packets/Notice.h"
#include "NodeLog.h"
#include "plugins/StorageHooks.h"
#include "Storage.h"

StorageHooks *StorageHooks::m_self = 0;

StorageHooks::StorageHooks(QObject *parent)
  : QObject(parent)
{
  if (!m_self)
    m_self = this;
  else
    add(this);
}


/*!
 * Создание нового канала типа SimpleID::ChannelId.
 * Эта функция будет вызвана только для только что созданных каналов, после добавления их в базу данных.
 *
 * \param channel Созданный канал.
 *
 * \sa Storage::channel(const QString &name).
 */
int StorageHooks::newChannelImpl(ChatChannel channel)
{
  if (m_self != this)
    return Notice::OK;

  SCHAT_LOG_TRACE(<< "HOOK: NEW CHANNEL" << (channel->name() + "/" + SimpleID::encode(channel->id())));

  channel->feeds().add(FeedStorage::create("topic"));

  foreach (StorageHooks *hook, m_hooks) {
    hook->newChannelImpl(channel);
  }

  return Notice::OK;
}


/*!
 * Создание нового канала типа SimpleID::UserId.
 * Эта функция будет вызвана только для только что созданных каналов, после добавления их в базу данных.
 *
 * \param channel Созданный канал.
 * \param data    Данные авторизационного пакета.
 * \param host    Адрес пользователя.
 *
 * \sa AnonymousAuth::auth().
 */
int StorageHooks::newUserChannelImpl(ChatChannel channel, const AuthRequest &data, const QString &host)
{
  if (m_self != this)
    return Notice::OK;

  SCHAT_LOG_TRACE(<< "HOOK: NEW USER CHANNEL" << (channel->name() + "@" + host + "/" + SimpleID::encode(channel->id())) << data.userAgent);

  qint64 date = DateTime::utc();

  channel->createAccount();
  channel->feeds().add(new AccountFeed(date));

  foreach (StorageHooks *hook, m_hooks) {
    hook->newUserChannelImpl(channel, data, host);
  }

  return Notice::OK;
}
