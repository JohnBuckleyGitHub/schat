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

#include <QDebug>

#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientHooks.h"
#include "client/SimpleClient.h"
#include "net/packets/ChannelPacket.h"
#include "net/SimpleID.h"

ClientChannels::ClientChannels(QObject *parent)
  : QObject(parent)
  , m_client(ChatClient::io())
{
  m_hooks = new Hooks::Channels(this);

  connect(m_client, SIGNAL(notice(int)), SLOT(notice(int)));
  connect(m_client, SIGNAL(idle()), SLOT(idle()));
}


/*!
 * Получение канал по идентификатору.
 *
 * \param id Идентификатор канала.
 * \return Канал или пустой канал, если поиск неудачен.
 */
ClientChannel ClientChannels::get(const QByteArray &id)
{
  return m_channels.value(id);
}


/*!
 * Запрос информации о каналах.
 */
bool ClientChannels::info(const QList<QByteArray> &channels)
{
  if (channels.isEmpty())
    return false;

  return m_client->send(ChannelPacket::info(ChatClient::id(), channels, m_client->sendStream()));
}


bool ClientChannels::join(const QByteArray &id)
{
  if (!Channel::isCompatibleId(id))
    return false;

  return m_client->send(ChannelPacket::join(ChatClient::id(), id, QString(), m_client->sendStream()));
}


/*!
 * Подключение к обычному каналу по имени.
 *
 * \param name Имя канала.
 */
bool ClientChannels::join(const QString &name)
{
  if (!Channel::isValidName(name))
    return false;

  return m_client->send(ChannelPacket::join(ChatClient::id(), QByteArray(), name, m_client->sendStream()));
}


/*!
 * Отключение от канала.
 */
bool ClientChannels::part(const QByteArray &id)
{
  if (!Channel::isCompatibleId(id))
    return false;

  return m_client->send(ChannelPacket::part(ChatClient::id(), id, m_client->sendStream()));
}


/*!
 * Слот вызывается, когда клиент прочитает все пришедшие пакеты.
 */
void ClientChannels::idle()
{
  if (!m_synced.isEmpty()) {
    emit channels(m_synced);
    m_synced.clear();
  }
}


/*!
 * Обработка получения нового уведомления.
 *
 * \param type Тип уведомления, должен быть равен Notice::ChannelType.
 */
void ClientChannels::notice(int type)
{
  if (type != Notice::ChannelType)
    return;

  ChannelPacket packet(type, ChatClient::io()->reader());
  if (!packet.isValid())
    return;

  m_packet = &packet;
  QString cmd = m_packet->command();
  qDebug() << cmd;

  if (cmd == "channel")
    channel();

  else if (cmd == "info")
    info();

  else if (cmd == "+")
    joined();

  else if (cmd == "-")
    part();

  else if (cmd == "quit")
    quit();

  emit notice(packet);
}


/*!
 * Добавление канала в таблицу каналов.
 */
ClientChannel ClientChannels::add()
{
  QByteArray id = m_packet->channelId();
  if (!Channel::isCompatibleId(id))
    return ClientChannel();

  ClientChannel channel = m_channels.value(id);
  if (!channel) {
    channel = ClientChannel(new Channel(id, m_packet->text()));
    m_channels[id] = channel;
  }

  channel->setName(m_packet->text());
  channel->gender() = m_packet->gender();
  channel->status() = m_packet->channelStatus();

  return channel;
}


/*!
 * Чтение заголовка канала.
 */
void ClientChannels::channel()
{
  ClientChannel channel = add();
  if (!channel)
    return;

  emit this->channel(channel->id());

  if (channel->type() == SimpleID::ChannelId) {
    channel->channels() = m_packet->channels();
    sync(channel);
  }

  m_synced += channel->id();
}


/*!
 * Чтение информации о канале.
 */
void ClientChannels::info()
{
  ClientChannel channel = add();
  if (!channel)
    return;

  m_synced += channel->id();
}


/*!
 * Обработка входа пользователя в канал.
 */
void ClientChannels::joined()
{
  ClientChannel user = add();
  if (!user)
    return;

  ClientChannel channel = get(m_packet->dest());
  channel->channels() += user->id();
  m_synced += user->id();

  emit joined(channel->id(), user->id());
}


void ClientChannels::part()
{
  ClientChannel user = get(m_packet->sender());
  if (!user)
    return;

  ClientChannel channel = get(m_packet->dest());
  if (!channel)
    return;

  emit part(channel->id(), user->id());

  channel->channels().remove(user->id());
}


void ClientChannels::quit()
{
  ClientChannel user = get(m_packet->sender());
  if (!user)
    return;

  emit quit(user->id());

  QHashIterator<QByteArray, ClientChannel> i(m_channels);
  while (i.hasNext()) {
    i.next();
    i.value()->channels().remove(user->id());
  }
}


/*!
 * Формирование запроса информации об не известных каналах.
 *
 * \param channel Указатель на канал.
 */
void ClientChannels::sync(ClientChannel channel)
{
  if (channel->type() != SimpleID::ChannelId)
    return;

  QList<QByteArray> channels = channel->channels().all();
  foreach (QByteArray id, channels) {
    if (m_channels.contains(id))
      m_synced += id;
    else
      m_unsynced += id;
  }

  info(m_unsynced);
}
