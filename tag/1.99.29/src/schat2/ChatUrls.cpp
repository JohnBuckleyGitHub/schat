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

#include <QDesktopServices>
#include <QTextDocument>

#include "ChatNotify.h"
#include "ChatUrls.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/SimpleClient.h"
#include "net/SimpleID.h"
#include "sglobal.h"

ChatUrls *ChatUrls::m_self = 0;

ChatUrls::ChatUrls(QObject *parent)
  : QObject(parent)
{
  m_self = this;
}


/*!
 * Получение канала из URL адреса.
 */
ClientChannel ChatUrls::channel(const QUrl &url)
{
  ClientChannel channel;
  if (url.scheme() != LS("chat") && url.host() != LS("channel"))
    return channel;

  QStringList path = ChatUrls::path(url);
  if (path.isEmpty())
    return channel;

  QByteArray id = SimpleID::decode(path.at(0).toLatin1());
  if (!Channel::isCompatibleId(id))
    return channel;

  channel = ChatClient::channels()->get(id);
  if (channel)
    return channel;

  channel = ClientChannel(new Channel(id, SimpleID::fromBase32(url.queryItemValue(LS("name")).toLatin1())));
  channel->gender().setRaw(url.queryItemValue(LS("gender")).toInt());
  if (!channel->isValid())
    return ClientChannel();

  return channel;
}


QStringList ChatUrls::actions(const QUrl &url)
{
  if (url.scheme() != LS("chat"))
    return QStringList();

  QStringList out = path(url);

  if (url.host() == LS("channel")) {
    if (out.size() < 2)
      return QStringList();

    out.removeFirst();
    return out;
  }

  return out;
}


QStringList ChatUrls::path(const QUrl &url)
{
  QString path = url.path(); // В некоторых случаях путь возвращается без начального /.
  if (path.startsWith(LC('/')))
    path.remove(0, 1);

  return path.split(LC('/'), QString::SkipEmptyParts);
}


/*!
 * Преобразует канал в URL адрес.
 *
 * \param channel Указатель на канал.
 * \param action  Действие над каналом.
 */
QUrl ChatUrls::toUrl(ClientChannel channel, const QString &action)
{
  QUrl out(LS("chat://channel"));
  out.setPath(SimpleID::encode(channel->id()) + (action.isEmpty() ? QString() : "/" + action));

  QList<QPair<QString, QString> > queries;
  queries.append(QPair<QString, QString>(LS("name"),   SimpleID::toBase32(channel->name().toUtf8())));
  queries.append(QPair<QString, QString>(LS("gender"), QString::number(channel->gender().raw())));

  out.setQueryItems(queries);

  return out;
}


void ChatUrls::openChannelUrl(const QUrl &url)
{
  QStringList actions = ChatUrls::actions(url);
  if (actions.isEmpty())
    return;

  ClientChannel channel = ChatUrls::channel(url);
  if (!channel)
    return;

  QString action = actions.first();

  if (action == LS("open")) {
    ChatNotify::start(Notify::OpenChannel, channel->id());
  }
  else if (action == LS("info")) {
    ChatNotify::start(Notify::OpenInfo, channel->id());
  }
  else if (action == LS("insert")) {
    ChatNotify::start(Notify::InsertText, QChar(QChar::Nbsp) + QString(LS("<a class=\"nick\" href=\"%1\">%2</a>")).arg(url.toString()).arg(Qt::escape(channel->name())) + QChar(QChar::Nbsp));
  }
  else if (action == LS("edit")) {
    if (actions.size() == 1)
      return;

    if (actions.at(1) == LS("topic") && channel->type() == SimpleID::ChannelId)
      ChatNotify::start(Notify::EditTopic, channel->id());
  }
}


void ChatUrls::openUrl(const QUrl &url)
{
  if (url.scheme() == LS("schat")) {
    ChatClient::io()->openUrl(url);
    return;
  }

  if (url.scheme() != LS("chat")) {
    QDesktopServices::openUrl(url);
    return;
  }

  if (url.host() == LS("channel")) {
    openChannelUrl(url);
  }
  else if (url.host() == LS("about")) {
    ChatNotify::start(Notify::OpenAbout);
  }
  else if (url.host() == LS("settings")) {
    ChatNotify::start(Notify::OpenSettings, url);
  }
}
