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

#include <QPainter>
#include <QTextDocument>

#include "ChatCore.h"
#include "ChatPlugins.h"
#include "client/SimpleClient.h"
#include "messages/AbstractMessage.h"
#include "net/SimpleID.h"
#include "plugins/AbstractHistory.h"
#include "ui/tabs/ChatView.h"
#include "ui/TabWidget.h"
#include "ui/UserUtils.h"

QHash<QByteArray, ClientUser> UserUtils::m_users;
QStringList UserUtils::m_colors;

UserUtils::UserUtils()
{
  m_colors += QLatin1String("default");
  m_colors += QLatin1String("black");
  m_colors += QLatin1String("gray");
  m_colors += QLatin1String("green");
  m_colors += QLatin1String("red");
  m_colors += QLatin1String("white");
  m_colors += QLatin1String("yellow");
  m_colors += QLatin1String("medical");
  m_colors += QLatin1String("nude");
  m_colors += QLatin1String("thief");
}


ClientUser UserUtils::user(const QByteArray &id)
{
  if (SimpleID::typeOf(id) != SimpleID::UserId)
    return ClientUser();

  return ClientUser();

//  ClientUser user = ChatCore::i()->client()->user(id);
//  if (user)
//    return user;
//
//  if (ChatCore::i()->plugins()->history()) {
//    user = ChatCore::i()->plugins()->history()->user(id);
//    if (user)
//      return user;
//  }

//  user = TabWidget::i()->user(id);
//  if (user)
//    return user;

//  return m_users.value(id);
}


/*!
 * Получение пользователя из url адреса.
 */
ClientUser UserUtils::user(const QUrl &url)
{
  ClientUser user;
  if (url.scheme() != QLatin1String("chat") && url.host() != QLatin1String("user"))
    return user;

  QString path = url.path(); // В некоторых случаях путь возвращается без начального /.
  if (path.startsWith(QLatin1String("/")))
    path.remove(0, 1);

  QByteArray id;

  if (path.contains(QLatin1String("/")))
    id = SimpleID::decode(path.split(QLatin1String("/")).at(0).toLatin1());
  else
    id = SimpleID::decode(path.toLatin1());

  if (SimpleID::typeOf(id) != SimpleID::UserId)
    return user;

  user = UserUtils::user(id);
  if (user)
    return user;

  user = ClientUser(new User());
  user->setId(id);
  user->setNick(SimpleID::fromBase32(url.queryItemValue("nick").toLatin1()));

  if (user->isValid()) {
    m_users[id] = user;
    return user;
  }

  return ClientUser();
}


ClientUser UserUtils::user(const QVariant &id)
{
  if (id.type() == QVariant::Url)
    return user(id.toUrl());
  else if (id.type() == QVariant::ByteArray)
    return user(id.toByteArray());

  return ClientUser();
}


/*!
 * Получение собственного идентификатора.
 *
 * \todo Реализовать получение идентификатора до подключения к серверу.
 */
QByteArray UserUtils::userId()
{
  return QByteArray();
//  return ChatCore::i()->client()->channelId();
}


/*!
 * Иконка пользователя.
 *
 * \param user    Пользователь.
 * \param status  true если необходимо отрисовывать статус.
 * \param offline true если необходимо отрисовывать статус User::OfflineStatus.
 */
QIcon UserUtils::icon(ClientUser user, bool status, bool offline)
{
  QString file = QLatin1String(":/images/user");
  int gender = user->gender().value();
  int color  = user->gender().color();

  if (gender == Gender::Unknown) {
    file += QLatin1String("-unknown");
  }
  else if (gender == Gender::Ghost) {
    file += QLatin1String("-ghost");
  }
  else if (gender == Gender::Bot) {
    file += QLatin1String("-bot");
  }
  else if (color != Gender::Default) {
    file += QLatin1String("-") + m_colors.at(color);
  }

  if (gender == Gender::Female)
    file += QLatin1String("-female");

  file += QLatin1String(".png");

  if (offline && user->status() == Status::Offline)
    status = true;

  if (status) {
    if (offline && user->status() == Status::Offline)
      return QIcon(QIcon(file).pixmap(16, 16, QIcon::Disabled));

//    return ChatCore::icon(file, overlay(user->status()));
  }

  return QIcon(file);
}


/*!
 * Имя статуса.
 */
QString UserUtils::statusTitle(int status)
{
  switch (status) {
    case Status::Offline:
      return QObject::tr("Offline");
      break;

    case Status::Away:
    case Status::AutoAway:
      return QObject::tr("Away");
      break;

    case Status::DnD:
      return QObject::tr("Do not disturb");
      break;

    case Status::FreeForChat:
      return QObject::tr("Free for Chat");
      break;

    default:
      return QObject::tr("Online");
      break;
  }
}


QString UserUtils::toolTip(ClientUser user)
{
  QString out = "<b>" + Qt::escape(user->nick()) + "</b><br />";
  out += QString("Id: %1").arg(QString(SimpleID::encode(user->id()))) + "<br />";
  out += Qt::escape(user->host()) + "<br />";
  out += Qt::escape(user->userAgent()) + "<br />";
  out += Qt::escape(user->account()) + "<br />";
  out += statusTitle(user->status()) + " " + Qt::escape(user->statusText()) + "<br />";
  out += QString("Server: %1").arg(user->serverNumber()) + "<br />";
  out += QString("Groups: %1").arg(user->groups().toString());

  return out;
}


/*!
 * Получение локального url адреса пользователя.
 */
QUrl UserUtils::toUrl(ClientUser user, const QString &action)
{
  QUrl out(QLatin1String("chat://user"));
  out.setPath(SimpleID::encode(user->id()) + (action.isEmpty() ? QString() : QLatin1String("/") + action));

  QList<QPair<QString, QString> > queries;
  queries.append(QPair<QString, QString>(QLatin1String("nick"), SimpleID::toBase32(user->nick().toUtf8())));

  out.setQueryItems(queries);

  return out;
}


void UserUtils::clear()
{
  m_users.clear();
}


void UserUtils::updateUserNick(ChatView *view, ClientUser user)
{
  QString param = '"' + SimpleID::encode(user->id()) + "\", ";
  param += '"' + UserUtils::toUrl(user, "insert").toString() + "\", ";
  param += '"' + AbstractMessage::quote(Qt::escape(user->nick())) + '"';

  view->evaluateJavaScript("updateUserNick", param);
}
