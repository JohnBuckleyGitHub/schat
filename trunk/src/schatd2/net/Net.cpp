/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include "net/Net.h"
#include "net/NetContext.h"
#include "net/NetReply.h"
#include "net/NetRequest.h"
#include "NodeLog.h"

#define LOG_N9010 LOG_TRACE("N9010", "Core/Net", "s:" << context.socket() << ". data:" << context.req()->toJSON())

Net::Net(QObject *parent)
  : QObject(parent)
{
}


void Net::req(const NetContext &context, NetReply &reply)
{
  LOG_N9010
}
