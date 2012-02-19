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

#include <QStringList>

#include "net/SimpleID.h"
#include "sglobal.h"
#include "text/MessageId.h"

MessageId::MessageId(const QString &id)
{
  QStringList in = id.split(LC(':'));
  if (in.size() > 0)
    m_date = in.at(0).toLongLong();

  if (in.size() > 1)
    m_id = SimpleID::decode(in.at(1).toLatin1());
}


MessageId::MessageId(qint64 date, const QByteArray &id)
  : m_id(id)
  , m_date(date)
{
}


bool MessageId::isValid() const
{
  if (m_date == 0)
    return false;

  if (SimpleID::typeOf(m_id) != SimpleID::MessageId)
    return false;

  return true;
}


QString MessageId::toString() const
{
  return QString::number(m_date) + LC(':') + SimpleID::encode(m_id);
}


QList<MessageId> MessageId::toList(const QString &ids)
{
  QStringList in = ids.split(LC(','), QString::SkipEmptyParts);
  QList<MessageId> out;

  for (int i = 0; i < in.size(); ++i) {
    MessageId id(in.at(i));
    if (id.isValid())
      out.append(id);
  }

  return out;
}


QString MessageId::toString(const QList<MessageId> &ids)
{
  QStringList out;
  for (int i = 0; i < ids.size(); ++i)
    out.append(ids.at(i).toString());

  return out.join(LS(","));
}
