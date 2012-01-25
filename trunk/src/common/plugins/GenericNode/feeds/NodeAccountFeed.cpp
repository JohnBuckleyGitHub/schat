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
#include "Channel.h"
#include "DataBase.h"
#include "DateTime.h"
#include "feeds/FeedStorage.h"
#include "feeds/NodeAccountFeed.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "sglobal.h"

NodeAccountFeed::NodeAccountFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
  m_header.acl().setMask(0444);
}


NodeAccountFeed::NodeAccountFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  m_header.acl().setMask(0444);
}


Feed* NodeAccountFeed::create(const QString &name)
{
  return new NodeAccountFeed(name, DateTime::utc());
}


Feed* NodeAccountFeed::load(const QString &name, const QVariantMap &data)
{
  return new NodeAccountFeed(name, data);
}


/*!
 * Обработка запросов.
 */
FeedQueryReply NodeAccountFeed::query(const QVariantMap &json, Channel *channel)
{
  if (!channel)
    return FeedQueryReply(Notice::Forbidden);

  if (head().channel()->id() != channel->id())
    return FeedQueryReply(Notice::Forbidden);

  QString action = json.value(LS("action")).toString();
  if (action.isEmpty())
    return FeedQueryReply(Notice::BadRequest);

  if (action == LS("reg"))
    return reg(json);

  return FeedQueryReply(Notice::NotImplemented);
}


void NodeAccountFeed::setChannel(Channel *channel)
{
  Feed::setChannel(channel);

  m_data[LS("account")] = channel->account()->name();
  m_data[LS("groups")]  = channel->account()->groups().toString();
}


/*!
 * Регистрация пользователя.
 */
FeedQueryReply NodeAccountFeed::reg(const QVariantMap &json)
{
  // Получение и проверка пароля.
  QByteArray password = SimpleID::decode(json.value(LS("pass")).toString().toLatin1());
  if (SimpleID::typeOf(password) != SimpleID::PasswordId)
    return FeedQueryReply(Notice::BadRequest);

  // Получение и проверка имени.
  QString name = json.value(LS("name")).toString().simplified().toLower().remove(LC(' '));
  int index = name.indexOf(LC('@'));
  if (index != -1)
    name = name.left(index);

  if (name.isEmpty())
    return FeedQueryReply(Notice::BadRequest);

  // Проверка имени на занятость.
  qint64 key = DataBase::accountKey(name);
  if (key != -1)
    return FeedQueryReply(Notice::ObjectAlreadyExists);

  head().channel()->account()->setName(name);
  head().channel()->account()->setPassword(password);
  m_data[LS("account")] = name;

  DataBase::update(Ch::channel(head().channel()->id()));
  FeedStorage::save(FeedPtr(this));

  FeedQueryReply reply   = FeedQueryReply(Notice::OK);
  reply.json[LS("name")] = name;
  return reply;
}
