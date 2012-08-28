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

#include "Ch.h"
#include "cores/Core.h"
#include "feeds/FeedStorage.h"
#include "JSON.h"
#include "net/PacketReader.h"
#include "net/packets/FeedNotice.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "NodeFeeds.h"
#include "sglobal.h"
#include "Sockets.h"
#include "Storage.h"

NodeFeeds::NodeFeeds(Core *core)
  : NodeNoticeReader(Notice::FeedType, core)
  , m_packet(0)
{
}


bool NodeFeeds::read(PacketReader *reader)
{
  if (SimpleID::typeOf(reader->sender()) != SimpleID::UserId)
    return false;

  m_user = Ch::channel(reader->sender(), SimpleID::UserId);
  if (!m_user)
    return false;

  m_channel = Ch::channel(reader->dest(), SimpleID::typeOf(reader->dest()));
  if (!m_channel)
    return false;

  FeedNotice packet(m_type, reader);
  m_packet = &packet;

  QString cmd = m_packet->command();
  int status = Notice::NotImplemented;

  if (cmd == LS("add") || cmd == LS("new"))
    status = add();
  else if (cmd == LS("clear"))
    status = clear();
  else if (cmd == LS("get"))
    status = get();
  else if (cmd == LS("headers"))
    status = headers();
  else if (cmd == LS("query"))
    status = query();
  else if (cmd == LS("remove") || cmd == LS("delete") || cmd == LS("del"))
    status = remove();
  else if (cmd == LS("revert"))
    status = revert();
  else if (cmd == LS("update"))
    status = update();

  if (status == Notice::OK)
    return false;

  reply(status);
  return false;
}


/*!
 * Обработка запроса пользователя на создание нового фида.
 *
 * В случае использования плагина "Raw Feeds" эта функция вызывается командой:
 * /feed add \<имя фида\> \<опциональные JSON данные фида\>.
 */
int NodeFeeds::add()
{
  QString name = m_packet->text();
  if (name.isEmpty())
    return Notice::BadRequest;

  if (!m_channel->canEdit(m_user))
    return Notice::Forbidden;

  FeedPtr feed = m_channel->feed(name, false);
  if (feed) {
    FeedStorage::clone(feed);
    reply(Notice::OK);
    return Notice::OK;
  }


  feed = m_channel->feed(name, true, false);
  if (!feed)
    return Notice::InternalError;

  if (!m_packet->raw().isEmpty()) {
    int status = feed->update(m_packet->json(), m_user.data());
    if (status != Notice::OK)
      return status;
  }

  feed->head().acl().add(m_user->id());

  int status = FeedStorage::save(feed);
  if (status == Notice::OK)
    reply(status);

  return status;
}


/*!
 * Обработка запроса пользователя на очистку данных фида.
 *
 * В случае использования плагина "Raw Feeds" эта функция вызывается командой:
 * /feed clear \<имя фида\>.
 */
int NodeFeeds::clear()
{
  int status = check(Acl::Write);
  if (status != Notice::OK)
    return status;

  FeedPtr feed = m_channel->feeds().all().value(m_packet->text());
  status = feed->clear(m_user.data());
  if (status != Notice::OK)
    return status;

  status = FeedStorage::save(feed);
  if (status == Notice::OK) {
    reply(status);
    get();
    broadcast(feed);
  }

  return status;
}


/*!
 * Получение тела фида или обработка GET запроса к данным фида.
 */
int NodeFeeds::get()
{
  const QString &text = m_packet->text();
  if (text.isEmpty())
    return Notice::BadRequest;

  QPair<QString, QString> request = split(text);
  FeedPtr feed = m_channel->feed(request.first, false);
  if (!feed)
    return Notice::NotFound;

  if (!Acl::canRead(feed.data(), m_user.data()))
    return Notice::Forbidden;

  if (!request.second.isEmpty())
    return get(feed, request.second);

  QVariantMap json = feed->feed(m_user.data());
  if (json.isEmpty())
    return Notice::Forbidden;

  if (m_packet->date() == feed->head().date())
    return Notice::NotModified;

  FeedPacket packet(new FeedNotice(m_packet->dest(), m_packet->sender(), LS("feed")));
  packet->setDirection(FeedNotice::Server2Client);
  packet->setText(request.first);
  packet->setData(Feed::merge(request.first, json));
  packet->setDate(feed->head().date());

  Core::send(packet);
  return Notice::OK;
}


/*!
 * Обработка \b get запроса к данным фида.
 */
int NodeFeeds::get(FeedPtr feed, const QString &request)
{
  const FeedReply reply = feed->get(request, m_packet->json(), m_user.data());
  if (reply.status != Notice::OK)
    return reply.status;

  FeedPacket packet(new FeedNotice(m_packet->dest(), m_packet->sender(), LS("get")));
  packet->setDirection(FeedNotice::Server2Client);
  packet->setText(m_packet->text());
  packet->setData(reply.json);

  if (reply.date)
    packet->setDate(reply.date);

  Core::send(packet);
  return Notice::OK;
}


/*!
 * Получение заголовков фидов для одиночного пользователя.
 *
 * В случае использования плагина "Raw Feeds" эта функция вызывается командой:
 * /feeds.
 */
int NodeFeeds::headers()
{
  m_core->send(m_user->sockets(), FeedNotice::reply(*m_packet, m_channel->feeds().headers(m_user.data())));
  return Notice::OK;
}


/*!
 * Обработка запроса пользователя к данным фида.
 *
 * В случае использования плагина "Raw Feeds" эта функция вызывается командой:
 * /feed query \<имя фида\> \<опциональные JSON данные запроса\>.
 */
int NodeFeeds::query()
{
  int status = check(Acl::Read);
  if (status != Notice::OK)
    return status;

  FeedPtr feed = m_channel->feed(m_packet->text(), false);
  FeedQueryReply reply = feed->query(m_packet->json(), m_user.data());
  if (reply.modified) {
    FeedStorage::save(feed, reply.date);
    reply.json[LS("date")] = feed->head().date();
    reply.json[LS("size")] = feed->head().data().value(LS("size"));
  }

  if (reply.status == Notice::OK) {
    if (!reply.single)
      m_core->send(m_user->sockets(), FeedNotice::reply(*m_packet, reply));
    else
      Core::send(FeedNotice::reply(*m_packet, reply));

    if (!reply.packets.isEmpty())
      Core::send(reply.packets);
  }

  if (reply.modified) {
    if (!reply.incremental)
      get();

    broadcast(m_channel->feed(m_packet->text(), false), !reply.incremental);
  }

  // В случае если статус ответа не равен Notice::OK и запрос содержал поле \b action добавляем это поле в ответ.
  if (reply.status != Notice::OK && m_packet->json().contains(LS("action"))) {
    FeedPacket packet = FeedNotice::reply(*m_packet, reply.status);
    QVariantMap data;
    data[LS("action")] = m_packet->json().value(LS("action"));
    packet->setData(data);

    m_core->send(m_user->sockets(), packet);
    return Notice::OK;
  }

  return reply.status;
}


/*!
 * Обработка запроса пользователя на удаление фида.
 * Для удаления фида необходимы права на редактирование.
 *
 * В случае использования плагина "Raw Feeds" эта функция вызывается командой:
 * /feed remove \<имя фида\>.
 */
int NodeFeeds::remove()
{
  int status = check(Acl::Edit);
  if (status != Notice::OK)
    return status;

  if (m_packet->text() == "acl")
    return Notice::BadRequest;

  FeedPtr feed = m_channel->feed(m_packet->text(), false);
  FeedStorage::remove(feed);
  m_channel->feeds().remove(m_packet->text());
  reply(status);
  return status;
}


/*!
 * Обработка запроса пользователя на откат фида.
 * Откат фида возможен, только если у пользователя есть права на редактирование фида.
 *
 * В случае использования плагина "Raw Feeds" эта функция вызывается командой:
 * /feed revert \<имя фида\> \<опциональный номер ревизии\>.
 */
int NodeFeeds::revert()
{
  int status = check(Acl::Edit);
  if (status != Notice::OK)
    return status;

  FeedPtr feed = m_channel->feed(m_packet->text(), false);
  status = FeedStorage::revert(feed, m_packet->json());
  if (status == Notice::OK) {
    reply(status);
    get();
    broadcast(m_channel->feed(m_packet->text(), false));
  }

  return status;
}


/*!
 * Обработка запроса пользователя на обновление данных фида.
 *
 * В случае использования плагина "Raw Feeds" эта функция вызывается командой:
 * /feed update \<имя фида\> \<JSON данные\>.
 */
int NodeFeeds::update()
{
  int status = check(Acl::Write);
  if (status != Notice::OK)
    return status;

  if (m_packet->raw().isEmpty())
    return Notice::BadRequest;

  FeedPtr feed = m_channel->feed(m_packet->text(), false);
  status = feed->update(m_packet->json(), m_user.data());
  if (status != Notice::OK)
    return status;

  status = FeedStorage::save(feed);
  if (status == Notice::OK) {
    reply(status);
    get();
    broadcast(feed);
  }

  return status;
}


/*!
 * Базовая проверка корректности запроса к фиду и проверка прав доступа.
 */
int NodeFeeds::check(int acl)
{
  const QString &name = m_packet->text();
  if (name.isEmpty())
    return Notice::BadRequest;

  FeedPtr feed = m_channel->feed(name, false);
  if (!feed)
    return Notice::NotFound;

  if (!feed->head().acl().can(m_user.data(), static_cast<Acl::ResultAcl>(acl)))
    return Notice::Forbidden;

  return Notice::OK;
}


/*!
 * Разделение строки на имя фида и запрос.
 *
 * Например, строка "server/uptime" будет разбита на "server" и "uptime".
 * Запрос может быть пустым, если нужно получить тело фида.
 */
QPair<QString, QString> NodeFeeds::split(const QString &text) const
{
  QPair<QString, QString> pair;
  int index = text.indexOf(LC('/'));
  if (index != -1) {
    pair.first  = text.mid(0, index);
    pair.second = text.mid(index + 1);
  }
  else
    pair.first = text;

  return pair;
}


/*!
 * Отравка заголовка фида всем если он публично доступен для чтения.
 */
void NodeFeeds::broadcast(FeedPtr feed, bool echo)
{
  QVariantMap json = Feed::merge(LS("f"), feed->head().f());
  if (json.isEmpty())
    return;

  FeedPacket packet = FeedNotice::reply(*m_packet, json);
  packet->setDest(m_channel->id());
  packet->setCommand(LS("headers"));
  m_core->send(Sockets::all(m_channel, m_user, echo), packet);
}


void NodeFeeds::reply(int status)
{
  m_core->send(m_user->sockets(), FeedNotice::reply(*m_packet, status));
}
