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

#include "alerts/MessageAlert.h"
#include "messages/ChannelMessage.h"
#include "net/packets/MessageNotice.h"
#include "net/SimpleID.h"
#include "sglobal.h"

MessageAlert::MessageAlert(const ChannelMessage &message)
  : Alert(LS("public"), message.packet()->id(), message.packet()->date())
{
  m_tab = message.tab();

  if (message.data().value(LS("Status")) == LS("referring"))
    m_type = LS("referring");
  else if (SimpleID::typeOf(m_tab) == SimpleID::UserId)
    m_type = LS("private");

  m_data[LS("Message")] = message.data();
}


MessageAlertType::MessageAlertType(const QString &type, int weight)
  : AlertType(type, weight)
{
  m_defaults[LS("popup")] = type != LS("public");
  m_defaults[LS("tray")]  = true;
  m_defaults[LS("sound")] = true;
  m_defaults[LS("file")]  = LS("Received.wav");

  if (type == LS("public"))
    m_icon = QIcon(LS(":/images/channel.png"));
  else
    m_icon = QIcon(LS(":/images/balloon.png"));
}


QString MessageAlertType::name() const
{
  if (m_type == LS("public"))
    return QObject::tr("Public message");
  else if (m_type == LS("referring"))
    return QObject::tr("Referring to you");
  else
    return QObject::tr("Private message");
}