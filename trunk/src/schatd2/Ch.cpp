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
#include "ChHook.h"
#include "cores/Core.h"
#include "DataBase.h"
#include "feeds/FeedStorage.h"
#include "net/packets/auth.h"
#include "net/packets/Notice.h"
#include "Normalize.h"
#include "Settings.h"
#include "sglobal.h"
#include "Storage.h"

Ch *Ch::m_self = 0;

Ch::Ch(QObject *parent)
  : QObject(parent)
{
  m_self = this;
}


Ch::~Ch()
{
  m_self = 0;
}


bool Ch::add(ChatChannel channel)
{
  if (DataBase::add(channel) == -1)
    return false;

  m_self->cache(channel);

  foreach (ChHook *hook, m_self->m_hooks) {
    hook->add(channel);
  }

  return true;
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
 * \param id       Идентификатор пользователя.
 * \param name     Новый ник.
 * \param override \b true если разрешена перезапись ника.
 *
 * \return \b true если обнаружена коллизия.
 */
bool Ch::isCollision(const QByteArray &id, const QString &name, bool override)
{
  int type = SimpleID::typeOf(id);
  QByteArray normalized = Normalize::toId(type, name);

  ChatChannel channel = Ch::channel(normalized, type, false);
  if (channel && channel->id() != id)
    return channel->id() != id;

  if (override)
    return DataBase::isCollision(id, normalized);

  return DataBase::isCollision(id, normalized, type);
}


/*!
 * Получение канала по идентификатору.
 *
 * \param id   Идентификатор канала, либо идентификатор нормализированного имени, либо идентификатор cookie.
 * \param type Тип канала, этот параметр игнорируется, если идентификатор найден в кеше.
 * \param db   \b true если необходимо загрузить канал из базы если он не найден в кеше.
 */
ChatChannel Ch::channel(const QByteArray &id, int type, bool db)
{
  if (type == SimpleID::ServerId)
    return server();

  ChatChannel channel = m_self->channelImpl(id, type, db);
  m_self->sync(channel);

  return channel;
}


/*!
 * Получение или создание обычного канала по имени.
 *
 * \param name Имя канала.
 * \param user Пользователь запрашивающий вход или создание канала.
 */
ChatChannel Ch::channel(const QString &name, ChatChannel user)
{
  ChatChannel channel = m_self->channelImpl(name, user);
  m_self->sync(channel, user);

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

  if (!server->isSynced()) {
    foreach (ChHook *hook, m_self->m_hooks) {
      hook->server(server, created);
    }

    server->setSynced(true);
  }

  return server;
}


int Ch::rename(ChatChannel channel, const QString &name)
{
  QByteArray normalized = channel->normalized();
  if (isCollision(channel->id(), name))
    return Notice::ObjectAlreadyExists;

  if (!channel->setName(name))
    return Notice::BadRequest;

  m_self->m_channels.remove(normalized);
  m_self->m_channels[channel->normalized()] = channel;
  DataBase::add(channel);
  return Notice::OK;
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


void Ch::load()
{
  Ch::server();
  Ch::channel(QString(LS("Main")));

  foreach (ChHook *hook, m_self->m_hooks) {
    hook->load();
  }
}


void Ch::remove(ChatChannel channel)
{
  DataBase::add(channel);
  m_self->remove(channel->id());

  foreach (ChHook *hook, m_self->m_hooks) {
    hook->remove(channel);
  }
}


/*!
 * Создание нового или успешная авторизация существующего пользователя.
 *
 * \param channel Канал-пользователь.
 * \param data    Авторизационные данные.
 * \param host    Адрес пользователя.
 * \param created \b true если пользователь был создан.
 * \param socket  Сокет пользователя.
 */
void Ch::userChannel(ChatChannel channel, const AuthRequest &data, const QString &host, bool created, quint64 socket)
{
  if (!socket)
    socket = Core::socket();

  channel->hosts()->add(HostInfo(new Host(data, host, socket)));

  foreach (ChHook *hook, m_self->m_hooks) {
    hook->userChannel(channel, data, host, created, socket);
    hook->userChannel(channel);
  }

  m_self->cache(channel);
  channel->setSynced(true);
}


/*!
 * Создание при необходимости фида в обычном канале.
 *
 * \param channel Обычный канал.
 * \param name    Имя фида.
 * \param user    Пользователь.
 */
void Ch::addNewFeedIfNotExist(ChatChannel channel, const QString &name, ChatChannel user)
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
void Ch::addNewUserFeedIfNotExist(ChatChannel channel, const QString &name)
{
  FeedPtr feed = channel->feed(name, false);
  if (feed)
    return;

  feed = channel->feed(name, true, false);
  feed->head().acl().add(channel->id());

  FeedStorage::save(feed);
}


/*!
 * Получение канала по идентификатору.
 *
 * \param id   Идентификатор канала, либо идентификатор нормализированного имени, либо идентификатор cookie.
 * \param type Тип канала, этот параметр игнорируется, если идентификатор найден в кеше.
 * \param db   \b true если необходимо загрузить канал из базы если он не найден в кеше.
 */
ChatChannel Ch::channelImpl(const QByteArray &id, int type, bool db)
{
  ChatChannel channel = m_channels.value(id);
  if (channel || !db)
    return channel;

  channel = DataBase::channel(id, type);
  if (channel)
    cache(channel);

  return channel;
}



/*!
 * Получение или создание обычного канала по имени.
 */
ChatChannel Ch::channelImpl(const QString &name, ChatChannel user)
{
  QByteArray normalized = Normalize::toId(LC('#') + name);
  ChatChannel channel = channelImpl(normalized);
  if (!channel)
    channel = channelImpl(makeId(normalized));

  if (!channel) {
    channel = ChatChannel(new ServerChannel(makeId(normalized), name));
    add(channel);

    foreach (ChHook *hook, m_hooks) {
      hook->newChannel(channel, user);
    }
  }

  return channel;
}


/*!
 * Добавление канала в кеш.
 */
void Ch::cache(ChatChannel channel)
{
  if (!channel)
    return;

  if (channel->type() != SimpleID::ServerId)
    Ch::server()->channels().add(channel->id());

  m_channels[channel->id()] = channel;
  m_channels[channel->normalized()] = channel;

  if (channel->account())
    m_channels[channel->account()->cookie] = channel;
}


void Ch::remove(const QByteArray &id)
{
  ChatChannel channel = m_channels.value(id);
  if (!channel)
    return;

  m_channels.remove(channel->id());
  m_channels.remove(channel->normalized());

  if (channel->account())
    m_channels.remove(channel->account()->cookie);
}


/*!
 * Синхронизация обычного канала.
 *
 * \param channel Указатель на канал.
 * \param user    Пользователь создавший канал, если есть такой.
 */
void Ch::sync(ChatChannel channel, ChatChannel user)
{
  if (channel && channel->type() == SimpleID::ChannelId && !channel->isSynced()) {
    foreach (ChHook *hook, m_hooks) {
      hook->sync(channel, user);
    }

    channel->setSynced(true);
  }
}
