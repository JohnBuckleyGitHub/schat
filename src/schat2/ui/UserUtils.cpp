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
#include "net/SimpleID.h"
#include "plugins/AbstractHistory.h"
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


ClientUser UserUtils::user()
{
  return ChatCore::i()->client()->user();
}


ClientUser UserUtils::user(const QByteArray &id)
{
  if (id.isEmpty())
    return ClientUser();

  ClientUser user = ChatCore::i()->client()->user(id);
  if (user || !ChatCore::i()->plugins()->history())
    return user;

  user = ChatCore::i()->plugins()->history()->user(id);
  if (!user)
    user = m_users.value(id);

  return user;
}


/*!
 * Получение пользователя из url адреса.
 */
ClientUser UserUtils::user(const QUrl &url)
{
  ClientUser user;
  if (url.scheme() != QLatin1String("chat") || url.host() != QLatin1String("user"))
    return user;

  QString path = url.path(); // В некоторых случаях путь возвращается без начального /.
  if (path.startsWith(QLatin1String("/")))
    path.remove(0, 1);

  QByteArray id;

  if (path.contains(QLatin1String("/")))
    id = SimpleID::fromBase64(path.split(QLatin1String("/")).at(0).toLatin1());
  else
    id = SimpleID::fromBase64(path.toLatin1());

  if (SimpleID::typeOf(id) != SimpleID::UserId)
    return user;

  user = UserUtils::user(id);
  if (user)
    return user;

  user = ClientUser(new User());
  user->setId(id);
  user->setNick(SimpleID::fromBase64(url.queryItemValue("nick").toLatin1()));

  if (user->isValid()) {
    m_users[id] = user;
    return user;
  }
  else
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


int UserUtils::color(const QString &color)
{
  int index = m_colors.indexOf(color);
  if (index == -1)
    return User::Default;

  return index;
}


/*!
 * Получение собственного идентификатора.
 *
 * \todo Реализовать получение идентификатора до подключения к серверу.
 */
QByteArray UserUtils::userId()
{
  return ChatCore::i()->client()->userId();
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
  int gender = user->gender();
  int color  = user->color();

  if (gender == User::Unknown) {
    file += QLatin1String("-unknown");
  }
  else if (gender == User::Ghost) {
    file += QLatin1String("-ghost");
  }
  else if (gender == User::Bot) {
    file += QLatin1String("-bot");
  }
  else if (color != User::Default) {
    file += QLatin1String("-") + m_colors.at(color);
  }

  if (gender == User::Female)
    file += QLatin1String("-female");

  file += QLatin1String(".png");

  if (status) {
    if (offline && user->status() == User::OfflineStatus)
      return QIcon(QIcon(file).pixmap(16, 16, QIcon::Disabled));

    return ChatCore::icon(file, overlay(user->status()));
  }

  return QIcon(file);
}


QString UserUtils::overlay(int status)
{
  if (status == User::AwayStatus || status == User::AutoAwayStatus)
    return QLatin1String(":/images/away-small.png");
  else if (status == User::DnDStatus)
    return QLatin1String(":/images/dnd-small.png");
  else if (status == User::FreeForChatStatus)
    return QLatin1String(":/images/ffc-small.png");
  else
    return QLatin1String("");
}


/*!
 * Имя статуса.
 */
QString UserUtils::statusTitle(int status)
{
  switch (status) {
    case User::OfflineStatus:
      return QObject::tr("Offline");
      break;

    case User::AwayStatus:
    case User::AutoAwayStatus:
      return QObject::tr("Away");
      break;

    case User::DnDStatus:
      return QObject::tr("Do not disturb");
      break;

    case User::FreeForChatStatus:
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
  out += Qt::escape(user->host()) + "<br />";
  out += Qt::escape(user->userAgent()) + "<br />";
  out += statusTitle(user->status()) + " " + Qt::escape(user->statusText()) + "<br />";
  out += QString("Server: %1").arg(user->serverNumber());

  return out;
}


/*!
 * Получение локального url адреса пользователя.
 */
QUrl UserUtils::toUrl(ClientUser user, const QString &action)
{
  QUrl out(QLatin1String("chat://user"));
  out.setPath(SimpleID::toBase64(user->id()) + (action.isEmpty() ? QString() : QLatin1String("/") + action));

  QList<QPair<QString, QString> > queries;
  queries.append(QPair<QString, QString>(QLatin1String("nick"), SimpleID::toBase64(user->nick().toUtf8())));

  out.setQueryItems(queries);

  return out;
}


void UserUtils::clear()
{
  m_users.clear();
}
