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

#include "MasterDataCreator.h"
#include "net/NetRecord.h"
#include "sglobal.h"

bool MasterDataCreator::create(ChatChannel channel, const QString &path, NetRecord &record) const
{
  if (path == QString()) {
    QVariantMap map;
    map.insert(LS("name"),   channel->name());
    map.insert(LS("type"),   channel->type());
    map.insert(LS("status"), channel->status().value());
    map.insert(LS("gender"), channel->gender().value());
    map.insert(LS("color"),  channel->gender().color());

    record.date = channel->date();
    record.data = map;
    return true;
  }

  return false;
}


QStringList MasterDataCreator::paths() const
{
  QStringList out;
  out.append(QString());
  return out;
}
