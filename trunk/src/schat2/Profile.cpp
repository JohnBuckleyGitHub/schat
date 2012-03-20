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
#include "Profile.h"
#include "sglobal.h"

QStringList Profile::fields;
Profile *Profile::m_self = 0;

Profile::Profile(QObject *parent)
  : QObject(parent)
{
  if (m_self)
    add(this);
  else
    m_self = this;

  fields << LS("name")
         << LS("bday")
         << LS("city")
         << LS("country")
         << LS("tel")
         << LS("email")
         << LS("site")
         << LS("note");

  connect(ChatClient::i(), SIGNAL(ready()), SLOT(ready()));
}


QString Profile::translate(const QString &field)
{
  return field;
}


/*!
 * Возвращает поля доступные для добавления.
 */
QStringList Profile::available()
{
  return fields;
}


void Profile::ready()
{
  FeedPtr feed = ChatClient::channel()->feed(LS("profile"), false);
  if (!feed)
    ChatClient::feeds()->request(ChatClient::id(), LS("add"), LS("profile"));
}
