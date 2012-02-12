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

#include "DateTime.h"
#include "feeds/Feed.h"
#include "net/packets/FeedNotice.h"
#include "sglobal.h"

FeedNotice::FeedNotice()
  : Notice(QByteArray(), QByteArray(), QString())
{
  m_type = FeedType;
}


FeedNotice::FeedNotice(const QByteArray &sender, const QByteArray &dest, const QString &command, const QByteArray &id)
  : Notice(sender, dest, command, DateTime::utc(), id)
{
  m_type = FeedType;
}


FeedNotice::FeedNotice(quint16 type, PacketReader *reader)
  : Notice(type, reader)
{
}


/*!
 * Отправка клиентам тела фида.
 */
FeedPacket FeedNotice::feed(const FeedNotice &source, const QVariantMap &json)
{
  FeedPacket packet(new FeedNotice(source.dest(), source.sender(), LS("feed")));
  packet->setDirection(Server2Client);
  packet->setText(source.text());
  packet->setData(Feed::merge(source.text(), json));
  return packet;
}


/*!
 * Запрос клиентом заголовков фидов.
 *
 * \param user    Идентификатор пользователя.
 * \param channel Идентификатор канала.
 * \param stream  Поток записи пакета.
 */
FeedPacket FeedNotice::headers(const QByteArray &user, const QByteArray &channel)
{
  FeedPacket packet(new FeedNotice(user, channel, "headers"));
  return packet;
}


FeedPacket FeedNotice::reply(const FeedNotice &source, const FeedQueryReply &reply)
{
  FeedPacket packet(new FeedNotice(source.dest(), source.sender(), "reply"));
  packet->setDirection(Server2Client);
  packet->setText(source.text());
  packet->setStatus(reply.status);
  packet->setData(reply.json);
  return packet;
}


FeedPacket FeedNotice::reply(const FeedNotice &source, const QVariantMap &json)
{
  FeedPacket packet(new FeedNotice(source.dest(), source.sender(), source.command()));
  packet->setDirection(Server2Client);
  packet->setText(source.text());
  packet->setData(json);
  return packet;
}


/*!
 * Базовая функция формирования ответа за запрос клиента.
 *
 * \param source  Исходный пакет, полученный от клиента.
 * \param status  Код ответа на запрос.
 * \param stream  Поток записи пакета.
 */
FeedPacket FeedNotice::reply(const FeedNotice &source, int status)
{
  FeedPacket packet(new FeedNotice(source.dest(), source.sender(), source.command()));
  packet->setDirection(Server2Client);
  packet->setText(source.text());
  packet->setStatus(status);
  return packet;
}


/*!
 * Универсальная функция запроса клиента к серверу.
 *
 * \param user    Идентификатор пользователя.
 * \param channel Идентификатор канала, к которому предназначен запрос.
 * \param command Команда.
 * \param name    Имя фида.
 * \param stream  Поток записи пакета.
 * \param json    JSON данные запроса.
 */
FeedPacket FeedNotice::request(const QByteArray &user, const QByteArray &channel, const QString &command, const QString &name, const QVariantMap &json)
{
  FeedPacket packet(new FeedNotice(user, channel, command));
  packet->setText(name);
  packet->setData(json);
  return packet;
}
