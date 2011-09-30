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

#include "ChatCore.h"
#include "client/SimpleClient.h"
#include "net/SimpleID.h"
#include "ui/ChannelUtils.h"

ClientChannel ChannelUtils::channel(const QByteArray &id)
{
  if (SimpleID::typeOf(id) != SimpleID::ChannelId)
    return ClientChannel();

  ClientChannel channel = ChatCore::i()->client()->channel(id);
  if (channel)
    return channel;

  return channel;
}


ClientChannel ChannelUtils::channel(const QUrl &url)
{
  ClientChannel channel;
  if (url.scheme() != QLatin1String("chat") && url.host() != QLatin1String("channel"))
    return channel;

  QString path = url.path(); // В некоторых случаях путь возвращается без начального /.
  if (path.startsWith(QLatin1String("/")))
    path.remove(0, 1);

  QByteArray id;
  if (path.contains(QLatin1String("/")))
    id = SimpleID::fromBase64(path.split(QLatin1String("/")).at(0).toLatin1());
  else
    id = SimpleID::fromBase64(path.toLatin1());

  if (SimpleID::typeOf(id) != SimpleID::ChannelId)
    return channel;

  channel = ChannelUtils::channel(id);
  if (channel)
    return channel;

  channel = ClientChannel(new Channel(id, SimpleID::fromBase64(url.queryItemValue("nick").toLatin1())));
  if (channel->isValid()) {
    return channel;
  }

  return ClientChannel();
}


QUrl ChannelUtils::toUrl(ClientChannel channel, const QString &action)
{
  QUrl out(QLatin1String("chat://channel"));
  out.setPath(SimpleID::toBase64(channel->id()) + (action.isEmpty() ? QString() : QLatin1String("/") + action));

  QList<QPair<QString, QString> > queries;
  queries.append(QPair<QString, QString>(QLatin1String("name"), SimpleID::toBase64(channel->name().toUtf8())));

  out.setQueryItems(queries);

  return out;
}
