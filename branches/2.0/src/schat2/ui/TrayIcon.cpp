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
#include "ui/TrayIcon.h"
#include "ui/UserUtils.h"
#include "net/SimpleClient.h"

TrayIcon::TrayIcon(QObject *parent)
  : QSystemTrayIcon(parent)
{
  m_client = ChatCore::i()->client();

  connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
  connect(m_client, SIGNAL(userDataChanged(const QByteArray &)), SLOT(updateUserData(const QByteArray &)));

  setTrayIcon();
}


void TrayIcon::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
  if (reason == QSystemTrayIcon::Trigger || QSystemTrayIcon::MiddleClick) {
    ChatCore::i()->startNotify(ChatCore::ToggleVisibilityNotice);
  }
}


void TrayIcon::updateUserData(const QByteArray &userId)
{
  if (m_client->userId() == userId)
    setTrayIcon(m_client->user()->status());
}


void TrayIcon::setTrayIcon(int status)
{
  if (status == -1) {
    m_icon = SCHAT_ICON(SmallLogoIcon);
  }
  else {
    m_icon = ChatCore::icon(":/images/schat16.png", UserUtils::overlay(status));
  }

  setIcon(m_icon);
}
