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

  if (action == LS("login"))
    return login(json);
  else if (action == LS("reg"))
    return reg(json);

  return FeedQueryReply(Notice::NotImplemented);
}


void NodeAccountFeed::setChannel(Channel *channel)
{
  Feed::setChannel(channel);

  m_data[LS("account")] = channel->account()->name();
  m_data[LS("groups")]  = channel->account()->groups().all();
}


/*!
 * Авторизация пользователя.
 */
FeedQueryReply NodeAccountFeed::login(const QVariantMap &json)
{
  if (!head().channel()->account()->name().isEmpty())
    return FeedQueryReply(Notice::BadRequest);

  // Получение и проверка пароля.
  QByteArray password = this->password(json);
  if (password.isEmpty())
    return FeedQueryReply(Notice::BadRequest);

  // Получение и проверка имени.
  QString name = this->name(json);
  if (name.isEmpty())
    return FeedQueryReply(Notice::BadRequest);

  // Проверка имени на существование.
  qint64 key = DataBase::accountKey(name);
  if (key == -1)
    return FeedQueryReply(Notice::NotFound);

  Account account = DataBase::account(key);
  if (!account.isValid())
    return FeedQueryReply(Notice::InternalError);

  if (account.password() != password)
    return FeedQueryReply(Notice::Forbidden);

  // Формирования ответа.
  FeedQueryReply reply     = FeedQueryReply(Notice::OK);
  reply.json[LS("action")] = LS("login");
  reply.json[LS("nick")]   = DataBase::nick(account.channel());
  reply.json[LS("cookie")] = SimpleID::encode(account.cookie());
  return reply;
}


/*!
 * Регистрация пользователя.
 */
FeedQueryReply NodeAccountFeed::reg(const QVariantMap &json)
{
  if (!head().channel()->account()->name().isEmpty())
    return FeedQueryReply(Notice::BadRequest);

  // Получение и проверка пароля.
  QByteArray password = this->password(json);
  if (password.isEmpty())
    return FeedQueryReply(Notice::BadRequest);

  // Получение и проверка имени.
  QString name = this->name(json);
  if (name.isEmpty())
    return FeedQueryReply(Notice::BadRequest);

  // Проверка имени на занятость.
  qint64 key = DataBase::accountKey(name);
  if (key != -1)
    return FeedQueryReply(Notice::ObjectAlreadyExists);

  // Обновление данных.
  ChatChannel channel = Ch::channel(head().channel()->id());
  if (!channel)
    return FeedQueryReply(Notice::InternalError);

  channel->account()->setName(name);
  channel->account()->setPassword(password);
  channel->account()->groups().remove(LS("anonymous"));
  channel->account()->groups().add(LS("registered"));
  m_data[LS("account")] = name;
  m_data[LS("groups")]  = channel->account()->groups().all();
  setRecovery(LS("q"), json);
  setRecovery(LS("a"), json);

  DataBase::update(channel);

  // Формирования ответа.
  FeedQueryReply reply     = FeedQueryReply(Notice::OK);
  reply.json[LS("action")] = LS("reg");
  reply.json[LS("name")]   = name;
  reply.modified           = true;
  return reply;
}


QByteArray NodeAccountFeed::password(const QVariantMap &json) const
{
  QByteArray password = SimpleID::decode(json.value(LS("pass")).toString().toLatin1());
  if (SimpleID::typeOf(password) == SimpleID::PasswordId)
    return password;

  return QByteArray();
}


QString NodeAccountFeed::name(const QVariantMap &json) const
{
  QString name = json.value(LS("name")).toString().simplified().toLower().remove(LC(' '));
  int index = name.indexOf(LC('@'));
  if (index != -1)
    name = name.left(index);

  return name;
}


void NodeAccountFeed::setRecovery(const QString &type, const QVariantMap &json)
{
  if (!json.contains(type))
    return;

  QByteArray id = json.value(type).toByteArray();
  if (SimpleID::typeOf(SimpleID::decode(id)) != SimpleID::MessageId)
    return;

  m_data[type] = id;
}
