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

#include "BenchmarkClient.h"
#include "client/SimpleClient.h"
#include "net/SimpleID.h"

BenchmarkClient::BenchmarkClient(const QString &nick, QObject *parent)
  : QObject(parent)
{
  m_client = new SimpleClient(this);
  m_client->setNick(nick);
  m_client->setCookieAuth(false);
  m_client->setUniqueId(SimpleID::randomId(SimpleID::UniqueUserId));
}


BenchmarkClient::BenchmarkClient(const QVariantMap &auth, QObject *parent)
: QObject(parent)
{
  m_client = new SimpleClient(this);
  m_client->setNick(auth.value("nick").toString());
  m_client->setCookieAuth(true);
  m_client->setUniqueId(SimpleID::decode(auth.value("uniqueId").toByteArray()));

  m_cookie = SimpleID::decode(auth.value("cookie").toByteArray());
}


bool BenchmarkClient::open(const QString &url)
{
  return m_client->openUrl(url, m_cookie);
}
