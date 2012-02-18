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

#include "ChatNotify.h"
#include "sglobal.h"

ChatNotify *ChatNotify::m_self = 0;

FeedNotify::FeedNotify(int type, const QByteArray &channel, const QString &name, const QVariantMap &json, int status)
  : Notify(type)
  , m_status(status)
  , m_channel(channel)
  , m_name(name)
  , m_json(json)
{
}


bool FeedNotify::isFeed(const FeedNotify &notify, const QString &name, const QByteArray &id, const QString &action)
{
  if (notify.name() != name)
    return false;

  if (notify.channel() != id)
    return false;

  if (action.isEmpty())
    return true;

  if (notify.json().value(LS("action")) != action)
    return false;

  return true;
}

ChatNotify::ChatNotify(QObject *parent)
  : QObject(parent)
{
  m_self = this;
}


bool ChatNotify::isFeed(const Notify &notify, const QString &name, const QByteArray &id, const QString &action)
{
  QVariantMap data = notify.data().toMap();
  if (data.value(LS("name")) != name)
    return false;

  if (data.value(LS("id")) != id)
    return false;

  if (action.isEmpty())
    return true;
  else if (data.value(LS("data")).toMap().value(LS("action")) != action)
    return false;

  return true;
}


void ChatNotify::startNotify(const Notify &notify)
{
  emit this->notify(notify);
}
