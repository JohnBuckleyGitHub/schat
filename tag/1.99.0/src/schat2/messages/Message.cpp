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

#include <QDebug>

#include <QTextDocument>

#include "ChatUrls.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "messages/Message.h"
#include "net/SimpleID.h"
#include "SimpleJSon.h"

Message::Message()
{
}


QString Message::json() const
{
  QString json = SimpleJSon::generate(m_data);
//  qDebug() << json;

  json.remove('\n');
  json.remove('\r');
  return json;
}


void Message::author(const QByteArray &id)
{
  if (id.isEmpty())
    return;

  ClientChannel user = ChatClient::channels()->get(id);
  if (!user)
    return;

  QVariantMap author;
  author["Id"]      = SimpleID::encode(user->id());
  author["Name"]    = Qt::escape(user->name());
  author["Url"]     = ChatUrls::toUrl(user, "insert").toString();
  m_data["Author"]  = author;
}
