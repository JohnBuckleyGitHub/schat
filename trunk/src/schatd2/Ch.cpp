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

#include <QDebug>

#include "Account.h"
#include "Ch.h"
#include "DataBase.h"
#include "feeds/FeedStorage.h"
#include "net/packets/Notice.h"
#include "Normalize.h"
#include "Settings.h"
#include "sglobal.h"
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
 * Автоматическое удаление канала, если он больше не нужен.
 *
 * \return \b true если канал был удалён.
 */
bool Ch::gc(ChatChannel channel)
{
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
 * Проверка ника на коллизию.
 *
 * \param id   Идентификатор пользователя.
 * \param name Новый ник.
 *
 * \return \b true если обнаружена коллизия.
 */
bool Ch::isCollision(const QByteArray &id, const QString &name)
{
  int type = SimpleID::typeOf(id);

  ChatChannel channel = Ch::channel(Normalize::toId(type, name), type, false);
  if (channel && channel->id() != id)
    return channel->id() != id;

  return DataBase::isCollision(id, Normalize::toId(type, name), type);
}


/*!
 * Получение канала по идентификатору.
 */
ChatChannel Ch::channel(const QByteArray &id, int type, bool db)
{
  if (type == SimpleID::ServerId)
    return server();

  ChatChannel channel = m_self->channelImpl(id, type, db);
  if (channel && channel->type() == SimpleID::ChannelId && !channel->isSynced())
    m_self->channelImpl(channel);

  return channel;
}


ChatChannel Ch::channel(const QString &name, ChatChannel user)
{
  ChatChannel channel = m_self->channelImpl(name, user);
  if (channel && channel->type() == SimpleID::ChannelId && !channel->isSynced())
    m_self->channelImpl(channel, user);

  return channel;
}


/*!
 * Получение канала сервера.
 */
ChatChannel Ch::server()
{
  ChatChannel server = m_self->channelImpl(Storage::serverId(), SimpleID::ServerId);
  bool created = false;

  if (!server) {
    server = ChatChannel(new ServerChannel(Storage::serverId(), QString()));
    add(server);
    created = true;
  }

  if (!server->isSynced())
    m_self->serverImpl(server, created);

  return server;
}


/*!
 * Генерирование новой Cookie.
 */
QByteArray Ch::cookie()
{
  return SimpleID::randomId(SimpleID::CookieId, Storage::privateId());
}


/*!
 * Создание идентификатора канала.
 */
QByteArray Ch::makeId(const QByteArray &normalized)
{
  return SimpleID::make("channel:" + Storage::privateId() + normalized, SimpleID::ChannelId);
}


/*!
 * Создание идентификатора пользователя.
 */
QByteArray Ch::userId(const QByteArray &uniqueId)
{
  return SimpleID::make("anonymous:" + Storage::privateId() + uniqueId, SimpleID::UserId);
}


/*!
 * Переименование канала.
 */
int Ch::renameImpl(ChatChannel channel, const QString &name)
{
  QByteArray normalized = channel->normalized();
  if (isCollision(channel->id(), name))
    return Notice::ObjectAlreadyExists;

  if (!channel->setName(name))
    return Notice::BadRequest;

  m_cache.rename(channel, normalized);
  DataBase::update(channel);
  return Notice::OK;
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


/*!
 * Получение канала по идентификатору.
 *
 * \todo В случае получения пользовательского канала по нормализированному имени и если ник устарел, сбрасывать ник и возвращать пустой канал.
 *
 * \param id   Идентификатор канала, либо идентификатор нормализированного имени, либо идентификатор Cookie.
 * \param type Тип канала, этот параметр игнорируется, если идентификатор найден в кеше.
 * \param db   \b true если необходимо загрузить канал из базы если он не найден в кеше.
 */
ChatChannel Ch::channelImpl(const QByteArray &id, int type, bool db)
{
  if (m_self != this)
    return ChatChannel();

  ChatChannel channel = m_cache.channel(id);
  if (channel)
    return channel;

  if (!db)
    return channel;

  channel = DataBase::channel(id, type);
  if (channel)
    m_cache.add(channel);

  return channel;
}


/*!
 * Получение или создание обычного канала по имени.
 */
ChatChannel Ch::channelImpl(const QString &name, ChatChannel user)
{
  if (m_self != this)
    return ChatChannel();

  QByteArray normalized = Normalize::toId(LC('#') + name);
  ChatChannel channel = channelImpl(normalized);
  if (!channel)
    channel = channelImpl(makeId(normalized));

  if (!channel) {
    channel = ChatChannel(new ServerChannel(makeId(normalized), name));
    add(channel);
    newChannelImpl(channel, user);
  }

  return channel;
}


void Ch::channelImpl(ChatChannel channel, ChatChannel user)
{
  if (m_self != this)
    return;

  foreach (Ch *hook, m_hooks) {
    hook->channelImpl(channel, user);
  }

  channel->setSynced(true);
}


/*!
 * Загрузка основных каналов сервера.
 */
void Ch::loadImpl()
{
  if (m_self != this)
    return;

  Ch::server();
  Ch::channel(QString(LS("Main")));

  foreach (Ch *hook, m_hooks) {
    hook->loadImpl();
  }
}


/*!
 * Создание нового обычного канала.
 *
 * \param channel Созданный канал.
 * \param user    Пользователь создавший канал, если есть такой.
 */
void Ch::newChannelImpl(ChatChannel channel, ChatChannel user)
{
  if (m_self != this)
    return;

  foreach (Ch *hook, m_hooks) {
    hook->newChannelImpl(channel, user);
  }
}


/*!
 * Создание нового или успешная авторизация существующего пользователя.
 *
 * \param channel Канал-пользователь.
 * \param data    Авторизационные данные.
 * \param host    Адрес пользователя.
 * \param created \b true если пользователь был создан.
 */
void Ch::userChannelImpl(ChatChannel channel, const AuthRequest &data, const QString &host, bool created)
{
  if (m_self != this)
    return;

  foreach (Ch *hook, m_hooks) {
    hook->userChannelImpl(channel, data, host, created);
  }

  m_cache.add(channel);
  channel->setSynced(true);
}


/*!
 * Удаление канала.
 *
 * Сначала происходит обновление базы данных, затем канал удаляется из кеша.
 * После этого вызываются хуки.
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
 * Инициализация серверного канала.
 *
 * \param channel Канал сервера.
 * \param created \b true если канал был создан, фактически это происходит только при первом запуске сервера.
 */
void Ch::serverImpl(ChatChannel channel, bool created)
{
  if (m_self != this)
    return;

  foreach (Ch *hook, m_hooks) {
    hook->serverImpl(channel, created);
  }

  channel->setSynced(true);
}


/*!
 * Создание при необходимости фида в обычном канале.
 *
 * \param channel Обычный канал.
 * \param name    Имя фида.
 * \param user    Пользователь.
 */
void Ch::addNewFeedIsNotExist(ChatChannel channel, const QString &name, ChatChannel user)
{
  if (channel->type() != SimpleID::ChannelId)
    return;

  FeedPtr feed = channel->feed(name, false);
  if (feed)
    return;

  feed = channel->feed(name, true, false);
  if (user)
    feed->head().acl().add(user->id());

  FeedStorage::save(feed);
}


/*!
 * Создание при необходимости пользовательского фида.
 *
 * \param channel Канал-пользователь.
 * \param name    Имя фида.
 */
void Ch::addNewUserFeedIsNotExist(ChatChannel channel, const QString &name)
{
  FeedPtr feed = channel->feed(name, false);
  if (feed)
    return;

  feed = channel->feed(name, true, false);
  feed->head().acl().add(channel->id());

  FeedStorage::save(feed);
}


/*!
 * Добавление канала в кеш.
 */
void Ch::Cache::add(ChatChannel channel)
{
  if (!channel)
    return;

  if (channel->type() != SimpleID::ServerId)
    Ch::server()->channels().add(channel->id());

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

//  Ch::server()->channels().remove(channel->id());

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
