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
#include "net/SimpleID.h"
#include "sglobal.h"
#include "WebBridge.h"

WebBridge *WebBridge::m_self = 0;

WebBridge::WebBridge(QObject *parent)
  : QObject(parent)
{
  m_self = this;
}


QString WebBridge::channel(const QString &id)
{
  ClientChannel channel = ChatClient::channels()->get(SimpleID::decode(id.toLatin1()));
  if (!channel)
    return QString();

  QVariantMap data;
  data[LS("Id")]   = id;
  data[LS("Name")] = channel->name();
  data[LS("Url")]  = ChatUrls::toUrl(channel, LS("insert")).toString();

  return JSON::generate(data);
}
