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

#include "DateTime.h"
#include "net/packets/FeedPacket.h"

FeedPacket::FeedPacket()
  : Notice(QByteArray(), QByteArray(), QString())
{
  m_type = FeedType;
}


FeedPacket::FeedPacket(const QByteArray &sender, const QByteArray &dest, const QString &command, const QByteArray &id)
  : Notice(sender, dest, command, DateTime::utc(), id)
{
  m_type = FeedType;
}


FeedPacket::FeedPacket(quint16 type, PacketReader *reader)
  : Notice(type, reader)
{
}


QByteArray FeedPacket::clear(const QByteArray &user, const QByteArray &channel, const QString &name, QDataStream *stream)
{
  FeedPacket packet(user, channel, "clear");
  packet.setText(name);
  return packet.data(stream);
}


/*!
 * Отправка клиенту тела фида.
 */
QByteArray FeedPacket::feed(ClientChannel channel, ClientChannel user, const QString &name, QDataStream *stream)
{
  FeedPacket packet(channel->id(), user->id(), "feed");
  packet.setDirection(Server2Client);
  packet.setData(channel->feeds().feed(name, user.data()));
  packet.setText(name);

  if (packet.json().isEmpty()) {
    if (!channel->feeds().all().contains(name))
      packet.setStatus(Notice::NotFound);
    else
      packet.setStatus(Notice::Forbidden);
  }

  return packet.data(stream);
}


/*!
 * Запрос клиентом тела фида.
 *
 * \param user    Идентификатор пользователя.
 * \param channel Идентификатор канала.
 * \param stream  Поток записи пакета.
 */
QByteArray FeedPacket::get(const QByteArray &user, const QByteArray &channel, const QString &name, QDataStream *stream)
{
  FeedPacket packet(user, channel, "get");
  packet.setText(name);
  return packet.data(stream);
}


/*!
 * Отправка клиенту заголовков фидов.
 */
QByteArray FeedPacket::headers(ClientChannel channel, ClientChannel user, QDataStream *stream)
{
  FeedPacket packet(channel->id(), user->id(), "headers");
  packet.setDirection(Server2Client);
  packet.setData(channel->feeds().headers(user.data()));

  return packet.data(stream);
}


/*!
 * Запрос клиентом заголовков фидов.
 *
 * \param user    Идентификатор пользователя.
 * \param channel Идентификатор канала.
 * \param stream  Поток записи пакета.
 */
QByteArray FeedPacket::headers(const QByteArray &user, const QByteArray &channel, QDataStream *stream)
{
  FeedPacket packet(user, channel, "headers");
  return packet.data(stream);
}


QByteArray FeedPacket::update(const QByteArray &user, const QByteArray &channel, const QString &name, const QVariantMap &json, QDataStream *stream)
{
  FeedPacket packet(user, channel, "update");
  packet.setText(name);
  packet.setData(json);
  return packet.data(stream);
}


QByteArray FeedPacket::updated(const QByteArray &channel, const QByteArray &user, const QString &name, int status, QDataStream *stream)
{
  FeedPacket packet(channel, user, "updated");
  packet.setDirection(FeedPacket::Server2Client);
  packet.setText(name);
  packet.setStatus(status);
  return packet.data(stream);
}
