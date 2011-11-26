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

#include "Sockets.h"
#include "Storage.h"

void Sockets::merge(QList<quint64> &out, const QList<quint64> &sockets)
{
  foreach (quint64 socket, sockets) {
    if (!out.contains(socket))
      out.append(socket);
  }
}


QList<quint64> Sockets::channel(ChatChannel channel)
{
  QList<quint64> out;
  if (!channel)
    return out;

  QList<QByteArray> channels = channel->channels().all();
  for (int i = 0; i < channels.size(); ++i) {
    ChatChannel channel = Storage::i()->channel(channels.at(i), SimpleID::UserId);
    if (!channel)
      continue;

    merge(out, channel->sockets());
  }

  return out;
}
