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
#include "DateTime.h"
#include "net/DataCreator.h"
#include "net/Net.h"
#include "net/NetContext.h"
#include "net/NetReply.h"
#include "net/NetRequest.h"
#include "NodeLog.h"
#include "sglobal.h"

#define LOG_N9010 LOG_TRACE("N9010", "Core/Net", "s:" << context.socket() << ". data:" << context.req()->toJSON())
#define LOG_N9011 LOG_ERROR("N9011", "Core/Net", "s:" << context.socket() << ". channel: \"" << destId << "\" not found")

Net::Net(QObject *parent)
  : QObject(parent)
{
}


void Net::add(DataCreator *creator)
{
  m_creators.add(creator);
}


void Net::pub(ChatChannel channel, const QString &path)
{
  if (!channel)
    return;

  DataCreator *creator = m_creators.get(path);
  if (!creator)
    return;

  NetRecord record;
  if (!creator->create(channel, path, record))
    return;

  if (!record.date)
    record.date = DateTime::utc();

  pub(channel->id(), path, record);
}


void Net::pub(const ChatId &id, const QString &path, const NetRecord &record)
{
  m_data[id].insert(path, record);
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

  if (!m_dest) {
    LOG_N9011
    return;
  }

  if (context.req()->method == NetRequest::GET)
    get(context, reply);
}


Net::Creators::~Creators()
{
  qDeleteAll(m_list);
}


void Net::Creators::add(DataCreator *creator)
{
  if (m_list.contains(creator))
    return;

  m_list.append(creator);
  const QStringList paths = creator->paths();
  foreach (const QString &path, paths)
    m_map.insert(path, creator);
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

  if (map.contains(path)) {
    const NetRecord &record = map[path];
    reply.date   = record.date;
    reply.status = NetReply::OK;
    reply.data   = record.data;
    return true;
  }

  return false;
}
