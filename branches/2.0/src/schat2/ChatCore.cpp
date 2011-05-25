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
QStringList ChatCore::m_icons;

ChatCore::ChatCore(QObject *parent)
  : QObject(parent)
{
  m_self = this;

  m_client = new SimpleClient(new User("IMPOMEZIA"), 0, this);
  #if defined(SCHAT_RANDOM_CLIENT_ID)
  m_client->user()->setNick(QUuid::createUuid().toString().mid(1, 8));
  #endif

  m_icons += "channel";
  m_icons += "gear";
  m_icons += "main-tab-menu";
  m_icons += "network-error";
  m_icons += "offline";
  m_icons += "online";
  m_icons += "plug";
  m_icons += "plug-disconnect";
  m_icons += "quit";
  m_icons += "secure";
  m_icons += "settings";
  m_icons += "sound";
  m_icons += "sound_mute";
  m_icons += "users";
  m_icons += "schat16";
  m_icons += "schat16-ny";
  m_icons += "text-bold";
  m_icons += "text-italic";
  m_icons += "text-strikethrough";
  m_icons += "text-underline";
  m_icons += "send";
}


QIcon ChatCore::icon(IconName name)
{
  if (name < m_icons.size())
    return QIcon(":/images/" + m_icons.at(name) + ".png");

  return QIcon();
}
