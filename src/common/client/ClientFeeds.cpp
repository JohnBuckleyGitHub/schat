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

#include "client/ChatClient.h"
#include "client/ClientFeeds.h"
#include "client/ClientHooks.h"
#include "client/SimpleClient.h"
#include "net/packets/FeedNotice.h"
#include "net/packets/Notice.h"
#include "sglobal.h"

ClientFeeds::ClientFeeds(QObject *parent)
  : QObject(parent)
{
  m_hooks = new Hooks::Feeds(this);

  connect(ChatClient::io(), SIGNAL(notice(int)), SLOT(notice(int)));
}


/*!
 * Запрос заголовков фидов.
 *
 * \param id Идентификатор канала, к которому предназначен запрос.
 */
bool ClientFeeds::headers(const QByteArray &id)
{
  if (!Channel::isCompatibleId(id))
    return false;

  return ChatClient::io()->send(FeedNotice::headers(ChatClient::id(), id));
}


bool ClientFeeds::query(const QString &name, const QString &action, const QVariantMap &json)
{
  return query(ChatClient::id(), name, action, json);
}


bool ClientFeeds::query(const QByteArray &id, const QString &name, const QString &action, const QVariantMap &json)
{
  QVariantMap data = json;
  data[LS("action")] = action;
  return request(id, LS("query"), name, data);
}


bool ClientFeeds::request(const QByteArray &id, const QString &command, const QString &name, const QVariantMap &json)
{
  if (!Channel::isCompatibleId(id))
    return false;

  if (command.isEmpty())
    return false;

  if (name.isEmpty())
    return false;

  return ChatClient::io()->send(FeedNotice::request(ChatClient::id(), id, command, name, json));
}


bool ClientFeeds::revert(const QByteArray &id, const QString &name, qint64 rev)
{
  QVariantMap json;
  if (rev > 0)
    json[LS("rev")] = rev;

  return request(id, LS("revert"), name, json);
}


void ClientFeeds::notice(int type)
{
  if (type != Notice::FeedType)
    return;

  FeedNotice packet(type, ChatClient::io()->reader());
  if (!packet.isValid())
    return;

  m_packet = &packet;
  m_hooks->readFeed(packet);
}
