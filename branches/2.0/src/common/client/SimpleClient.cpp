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

#include "debugstream.h"

#include "client/SimpleClient.h"
#include "client/SimpleClient_p.h"
#include "net/packets/auth.h"

SimpleClientPrivate::SimpleClientPrivate()
{
}


SimpleClientPrivate::~SimpleClientPrivate()
{
}


SimpleClient::SimpleClient(QObject *parent)
  : AbstractClient(*new SimpleClientPrivate(), parent)
{
}


SimpleClient::SimpleClient(SimpleClientPrivate &dd, QObject *parent)
  : AbstractClient(dd, parent)
{
}


SimpleClient::~SimpleClient()
{
}


void SimpleClient::requestAuth()
{
  SCHAT_DEBUG_STREAM(this << "requestAuth()")

  Q_D(SimpleClient);

  AuthRequestData data(AuthRequestData::Anonymous, d->url.host(), d->user.data());
  data.uniqueId = d->uniqueId;
  send(AuthRequestWriter(d->sendStream, data).data());
}
