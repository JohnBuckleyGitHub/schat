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

#include "client/ChatClient.h"
#include "DateTime.h"
#include "messages/Message.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "WebBridge.h"

Message::Message()
{
}


Message::Message(const QByteArray &id, const QByteArray &tab, const QString &type, const QString &func)
  : m_tab(tab)
{
  setId(id);

  if (!type.isEmpty())
    m_data[LS("Type")] = type;

  if (!type.isEmpty())
    m_data[LS("Func")] = func;
}


/*!
 * Определение идентификатора вкладки в зависимости от отправителя и получателя сообщения.
 */
QByteArray Message::detectTab(const QByteArray &sender, const QByteArray &dest)
{
  if (SimpleID::typeOf(dest) == SimpleID::ChannelId || sender == ChatClient::id())
    return dest;

  return sender;
}


/*!
 * Установка автора сообщения.
 */
void Message::setAuthor(const QByteArray &id)
{
  QVariantMap data = WebBridge::channel(id);

  if (!data.isEmpty())
    m_data[LS("Author")] = data;
}


void Message::setDate(qint64 date)
{
  if (!date)
    date = DateTime::utc();

  m_data[LS("Date")] = date;
}


/*!
 * Установка идентификатора сообщения.
 */
void Message::setId(const QByteArray &id)
{
  m_data[LS("Id")] = QString(SimpleID::encode(id));
}
