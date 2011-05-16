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
#include "net/SimpleClient.h"
#include "User.h"

#define SCHAT_RANDOM_CLIENT_ID
#include <QUuid>

ChatCore *ChatCore::m_self = 0;

ChatCore::ChatCore(QObject *parent)
  : QObject(parent)
{
  m_self = this;

  m_client = new SimpleClient(new User("IMPOMEZIA"), 0, this);
  #if defined(SCHAT_RANDOM_CLIENT_ID)
  m_client->user()->setNick(QUuid::createUuid().toString().mid(1, 8));
  #endif
}
