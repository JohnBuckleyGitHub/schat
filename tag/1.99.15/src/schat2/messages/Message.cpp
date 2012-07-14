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

#include "ChatUrls.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "JSON.h"
#include "messages/Message.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "WebBridge.h"

Message::Message()
  : m_func(LS("addMessage"))
{
}


QString Message::json() const
{
  QString json = JSON::generate(m_data);

  json.remove(LC('\n'));
  json.remove(LC('\r'));
  return json;
}


void Message::author(const QByteArray &id)
{
  QVariantMap data = WebBridge::channel(id);

  if (!data.isEmpty())
    m_data[LS("Author")] = data;
}