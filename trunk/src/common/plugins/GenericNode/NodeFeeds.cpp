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

#define SCHAT_CHECK_ACL(x) CheckResult result = check(x);   \
                           if (result.status != Notice::OK) \
                             return result.status;          \


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

  if (cmd == LS("get"))
    status = get();
  else if (cmd == LS("put") || cmd == "post" || cmd == "delete")
    status = query(cmd);
  else if (cmd == LS("headers"))
    status = headers();
  else if (cmd == LS("query"))
    status = query();
  else if (cmd == LS("remove"))
    status = remove();

  if (status == Notice::OK)
    return false;

  reply(status);
  return false;
}


NodeFeeds::CheckResult::CheckResult(const QString &text)
  : status(Notice::OK)
{
  if (!text.isEmpty()) {
    QPair<QString, QString> split = FeedNotice::split(text);
    name = split.first;
    path = split.second;
  }

  if (name.isEmpty())
    status = Notice::BadRequest;
}


/*!
 * Обработка запроса пользователя на создание нового фида.
 *
 * Эта функция вызывается \b post запросом содержащим только имя фида.
 */
FeedReply NodeFeeds::add()
{
  FeedReply reply(Notice::OK);
  const QString &name = m_packet->text();
  if (name.contains(LC('*')) || name.contains(LC('/')))
    return Notice::BadRequest;

  FeedPtr feed        = m_channel->feed(name, false);
  if (!feed) {
    feed = m_channel->feed(name, true, false);
    if (!feed)
      return Notice::InternalError;

    feed->head().acl().add(m_user->id());
    reply.status = FeedStorage::save(feed);
  }
  else
    FeedStorage::clone(feed);

  reply.status = FeedStorage::save(feed);
  if (reply.status == Notice::OK)
    reply.date = feed->head().date();

  return reply;
}


FeedReply NodeFeeds::post(CheckResult &result)
{
  if (result.path.isEmpty()) {
    FeedReply reply = add();
    if (reply.status == Notice::OK) {
      result.feed = m_channel->feed(result.name, false);
      if (!result.feed)
        return Notice::InternalError;
    }

    return reply;
  }
  else if (result.feed)
    return result.feed->post(result.path, m_packet->json(), m_user.data());
  else
    return Notice::NotFound;
}


/*!
 * Получение тела фида или обработка \b get запроса к данным фида.
 */
int NodeFeeds::get()
{
  SCHAT_CHECK_ACL(Acl::Read)

  if (!result.path.isEmpty())
    return get(result.feed, result.path);

  if (m_packet->date() == result.feed->head().date())
    return Notice::NotModified;

  FeedPacket packet(new FeedNotice(m_packet->dest(), m_packet->sender(), LS("feed")));
  packet->setDirection(FeedNotice::Server2Client);
  packet->setText(result.name);
  packet->setData(Feed::merge(result.name, result.feed->feed(m_user.data())));
  packet->setDate(result.feed->head().date());

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
  if (!reply.packets.isEmpty())
    Core::send(reply.packets);

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
 *
 * \deprecated
 */
int NodeFeeds::query()
{
  SCHAT_CHECK_ACL(Acl::Read)

  FeedPtr feed = result.feed;
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
 * Обработка \b put, \b post или \b delete запросов.
 */
int NodeFeeds::query(const QString &verb)
{
  SCHAT_CHECK_ACL(Acl::Write)

  FeedReply reply(Notice::InternalError);
  if (verb == LS("put"))
    reply = result.feed->put(result.path, m_packet->json(), m_user.data());
  else if (verb == LS("post")) {
    reply = post(result);
  }
  else if (verb == LS("delete"))
    reply = result.feed->del(result.path, m_user.data());

  if (reply.status != Notice::OK)
    return reply.status;

  if (reply.date)
    FeedStorage::save(result.feed, reply.date);

  FeedPacket packet(new FeedNotice(m_packet->dest(), m_packet->sender(), verb));
  packet->setDirection(FeedNotice::Server2Client);
  packet->setText(m_packet->text());
  packet->setData(reply.json);
  packet->setDate(result.feed->head().date());

  int options = m_packet->json().value(LS("options")).toInt();
  if (options & Feed::Share)
    m_core->send(m_user->sockets(), packet);
  else
    Core::send(packet);

  if (reply.date && options & Feed::Broadcast)
    broadcast(result.feed, true);

  return Notice::OK;
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
  SCHAT_CHECK_ACL(Acl::Edit)

  if (m_packet->text() == LS("acl"))
    return Notice::BadRequest;

  FeedStorage::remove(result.feed);
  m_channel->feeds().remove(m_packet->text());
  reply(Notice::OK);
  return Notice::OK;
}


/*!
 * Базовая проверка корректности запроса к фиду и проверка прав доступа.
 */
NodeFeeds::CheckResult NodeFeeds::check(int acl)
{
  CheckResult result(m_packet->text());
  if (result.status != Notice::OK)
    return result;

  result.feed = m_channel->feed(result.name, false);
  if (!result.feed) {
    if (m_packet->command() == LS("post") && result.path.isEmpty()) {
      if (!m_channel->canEdit(m_user))
        result.status = Notice::Forbidden;
    }
    else
      result.status = Notice::NotFound;
  }
  else if (!result.feed->head().acl().can(m_user.data(), static_cast<Acl::ResultAcl>(acl)))
    result.status = Notice::Forbidden;

  return result;
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
