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

#include "Ch.h"
#include "net/Net.h"
#include "net/NetContext.h"
#include "net/NetReply.h"
#include "net/NetRequest.h"
#include "NodeLog.h"
#include "sglobal.h"

#define LOG_N9010 LOG_TRACE("N9010", "Core/Net", "s:" << context.socket() << ". data:" << context.req()->toJSON())

Net::Net(QObject *parent)
  : QObject(parent)
{
}


void Net::req(const NetContext &context, NetReply &reply)
{
  LOG_N9010

  const QString destId = context.req()->request.section(LC('/'), 0, 0);

  if (destId == LS("server")) {
    m_dest = Ch::server();
  }
  else {
    ChatId id(destId);
    if (!id.isNull())
      m_dest = Ch::channel(id.toByteArray(), id.type());
  }

  if (!m_dest)
    return;

  if (context.req()->method == NetRequest::GET)
    get(context, reply);
}


/*!
 * \todo Добавить проверку прав доступа.
 * \todo Добавить поддержку хуков.
 */
bool Net::get(const NetContext &context, NetReply &reply) const
{
  Q_UNUSED(reply)

  const NetRecordMap &map = m_data[m_dest->id()];
  const QString path      = context.req()->request.section(LC('/'), 1);

  if (!map.contains(path))
    return false;

  reply.data   = map[path].data;
  reply.status = NetReply::OK;
  return true;
}
