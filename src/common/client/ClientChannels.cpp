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
  connect(m_client, SIGNAL(restore()), SLOT(restore()));
  connect(m_client, SIGNAL(setup()), SLOT(setup()));
}


/*!
 * Получение канал по идентификатору.
 *
 * \param id Идентификатор канала.
 * \return Канал или пустой канал, если поиск неудачен.
 */
ClientChannel ClientChannels::get(const QByteArray &id)
{
  if (ChatClient::id() == id) {
    m_channels[id] = ChatClient::channel();
    return ChatClient::channel();
  }

  ClientChannel channel = m_channels.value(id);
  if (!channel) {
    channel = m_hooks->get(id);

    if (channel)
      m_channels[id] = channel;
  }

  return channel;
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


bool ClientChannels::nick(const QString &nick)
{
  ClientChannel user(new Channel(ChatClient::channel()->id(), nick));
  user->gender() = ChatClient::channel()->gender().raw();
  user->status() = ChatClient::channel()->status().value();

  if (!user->isValid())
    return false;

  return m_client->send(ChannelPacket::update(user, m_client->sendStream()));
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


bool ClientChannels::update()
{
  return m_client->send(ChannelPacket::update(ChatClient::channel(), m_client->sendStream()));
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
  qDebug() << cmd << m_packet->text();

  if (cmd == "channel")
    channel();

  else if (cmd == "info")
    add();

  else if (cmd == "+")
    joined();

  else if (cmd == "-")
    part();

  else if (cmd == "quit")
    quit();

  emit notice(packet);
}


void ClientChannels::restore()
{
  qDebug() << "** RESTORE **";
}


void ClientChannels::setup()
{
  qDebug() << "** SETUP **";
  m_channels.clear();
  m_synced.clear();
  m_unsynced.clear();
}


/*!
 * Добавление канала в таблицу каналов.
 */
ClientChannel ClientChannels::add()
{
  QByteArray id = m_packet->channelId();
  if (!Channel::isCompatibleId(id))
    return ClientChannel();

  m_unsynced.removeAll(id);
  ClientChannel channel = get(id);
  ChannelInfo info(id);

  if (!channel) {
    channel = ClientChannel(new Channel(id, m_packet->text()));
    m_channels[id] = channel;
  }
  else
    info.setOption(ChannelInfo::Updated);

  QString name = channel->name();
  channel->setName(m_packet->text());
  if (channel->name() != name)
    info.setOption(ChannelInfo::Renamed);

  channel->gender() = m_packet->gender();
  channel->status() = m_packet->channelStatus();

  m_synced += channel->id();

  m_hooks->add(info);
  emit this->channel(info);

  return channel;
}


/*!
 * Чтение заголовка канала.
 */
void ClientChannels::channel()
{
  ClientChannel channel = add();
  emit this->channel(channel->id());

  if (channel->type() == SimpleID::ChannelId) {
    channel->channels() = m_packet->channels();
    sync(channel);
  }
}


/*!
 * Обработка входа пользователя в канал.
 */
void ClientChannels::joined()
{
  ClientChannel user = add();
  ClientChannel channel = get(m_packet->dest());

  channel->channels() += user->id();

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
