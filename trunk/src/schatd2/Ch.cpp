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
#include "Ch.h"
#include "DataBase.h"
#include "net/ServerData.h"
#include "Normalize.h"
#include "Storage.h"

Ch *Ch::m_self = 0;

Ch::Ch(QObject *parent)
  : QObject(parent)
{
  if (!m_self)
    m_self = this;
  else
    add(this);
}


/*!
 * Проверка ника на коллизию.
 *
 * \param id   Идентификатор пользователя.
 * \param name Новый ник.
 *
 * \return \b true если обнаружена коллизия.
 */
bool Ch::isCollision(const QByteArray &id, const QString &name)
{
  ChatChannel channel = Ch::channel(Normalize::toId('~' + name), SimpleID::UserId);
  if (channel && channel->id() != id) {
    Ch::gc(channel);
    return true;
  }

  return false;
}


/*!
 * Создание идентификатора канала.
 */
QByteArray Ch::makeId(const QByteArray &normalized)
{
  return SimpleID::make("channel:" + Storage::serverData()->privateId() + normalized, SimpleID::ChannelId);
}


/*!
 * Добавление канала.
 */
bool Ch::addImpl(ChatChannel channel)
{
  if (m_self != this)
    return false;

  if (DataBase::add(channel) == -1)
    return false;

  m_cache.add(channel);

  foreach (Ch *hook, m_hooks) {
    hook->addImpl(channel);
  }

  return true;
}


bool Ch::gcImpl(ChatChannel channel)
{
  if (m_self != this)
    return false;

  if (channel->type() == SimpleID::UserId) {
    if (channel->sockets().size())
      return false;

    channel->status() = Status::Offline;
  }

  if (channel->channels().all().size())
    return false;

  remove(channel);
  return true;
}


/*!
 * Получение канала по идентификатору канала или идентификатору нормализированного имени либо Сookie.
 *
 * \todo В случае получения пользовательского канала по нормализированному имени и если ник устарел, сбрасывать ник и возвращать пустой канал.
 */
ChatChannel Ch::channelImpl(const QByteArray &id, int type)
{
  if (m_self != this)
    return ChatChannel();

  ChatChannel channel = m_cache.channel(id);
  if (channel)
    return channel;

  channel = DataBase::channel(id, type);
  if (channel)
    m_cache.add(channel);

  return channel;
}


/*!
 * Получение или создание обычного канала по имени.
 */
ChatChannel Ch::channelImpl(const QString &name)
{
  if (m_self != this)
    return ChatChannel();

  QByteArray normalized = Normalize::toId('#' + name);
  ChatChannel channel = this->channel(normalized);

  if (!channel) {
    channel = ChatChannel(new ServerChannel(makeId(normalized), name));
    add(channel);
    newChannelImpl(channel);
  }

  return channel;
}


void Ch::newChannelImpl(ChatChannel channel)
{
  if (m_self != this)
    return;

  foreach (Ch *hook, m_hooks) {
    hook->newChannelImpl(channel);
  }
}


void Ch::newUserChannelImpl(ChatChannel channel, const AuthRequest &data, const QString &host, bool created)
{
  if (m_self != this)
    return;

  foreach (Ch *hook, m_hooks) {
    hook->newUserChannelImpl(channel, data, host, created);
  }
}


/*!
 * Удаление канала.
 */
void Ch::removeImpl(ChatChannel channel)
{
  if (m_self != this)
    return;

  DataBase::update(channel);
  m_cache.remove(channel->id());

  foreach (Ch *hook, m_hooks) {
    hook->removeImpl(channel);
  }
}


/*!
 * Переименование канала.
 */
void Ch::renameImpl(ChatChannel channel, const QString &name)
{
  if (m_self != this)
    return;

  if (channel->type() != SimpleID::UserId)
    return;

  QByteArray normalized = channel->normalized();
  if (isCollision(channel->id(), name))
    return;

  channel->setName(name);
  m_cache.rename(channel, normalized);
  DataBase::update(channel);
}


/*!
 * Добавление канала в кеш.
 */
void Ch::Cache::add(ChatChannel channel)
{
  if (!channel)
    return;

  m_channels[channel->id()] = channel;
  m_channels[channel->normalized()] = channel;

  if (channel->account())
    m_channels[channel->account()->cookie()] = channel;
}


/*!
 * Удаление канала из кэша.
 */
void Ch::Cache::remove(const QByteArray &id)
{
  ChatChannel channel = this->channel(id);
  if (!channel)
    return;

  m_channels.remove(channel->id());
  m_channels.remove(channel->normalized());

  if (channel->account())
    m_channels.remove(channel->account()->cookie());

  return;
}


/*!
 * Переименование канала.
 */
void Ch::Cache::rename(ChatChannel channel, const QByteArray &before)
{
  m_channels.remove(before);
  m_channels[channel->normalized()] = channel;
}
