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

#include "DateTime.h"
#include "net/packets/MessageNotice.h"
#include "sglobal.h"

MessageNotice::MessageNotice()
  : Notice(QByteArray(), QByteArray(), LS("m"))
{
  m_type = MessageType;
}


MessageNotice::MessageNotice(const QByteArray &sender, const QByteArray &dest, const QString &text, quint64 time, const QByteArray &id)
  : Notice(sender, dest, LS("m"), time, id)
{
  m_type = MessageType;
  setText(text);

  if (time == 0)
    m_date = DateTime::utc();
}


MessageNotice::MessageNotice(quint16 type, PacketReader *reader)
  : Notice(type, reader)
{
}
