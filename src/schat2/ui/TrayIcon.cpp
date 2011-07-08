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

#include <QMenu>

#include "ChatCore.h"
#include "net/SimpleClient.h"
#include "ui/StatusMenu.h"
#include "ui/TrayIcon.h"
#include "ui/UserUtils.h"

TrayIcon::TrayIcon(QObject *parent)
  : QSystemTrayIcon(parent)
  , m_client(ChatCore::i()->client())
{
  m_menu = new QMenu();

  m_menu->addMenu(ChatCore::i()->statusMenu());
  m_menu->addSeparator();

  m_settingsAction = m_menu->addAction(SCHAT_ICON(SettingsIcon), tr("Preferences..."), this, SLOT(settings()));
  m_aboutAction = m_menu->addAction(SCHAT_ICON(SmallLogoIcon), tr("About..."), this, SLOT(about()));
  m_menu->addSeparator();

  m_quitAction = m_menu->addAction(SCHAT_ICON(QuitIcon), tr("Quit"), this, SLOT(quit()));

  setContextMenu(m_menu);

  connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
  connect(ChatCore::i()->statusMenu(), SIGNAL(updated()), SLOT(update()));

  update();
}


TrayIcon::~TrayIcon()
{
  delete m_menu;
}


void TrayIcon::retranslateUi()
{
  m_settingsAction->setText(tr("Preferences..."));
  m_aboutAction->setText(tr("About..."));
  m_quitAction->setText(tr("Quit"));
}


void TrayIcon::about()
{
  ChatCore::i()->startNotify(ChatCore::AboutNotice);
  ChatCore::i()->startNotify(ChatCore::ShowChatNotice);
}


void TrayIcon::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
  if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::MiddleClick) {
    ChatCore::i()->startNotify(ChatCore::ToggleVisibilityNotice);
  }
}


void TrayIcon::quit()
{
  ChatCore::i()->startNotify(ChatCore::QuitNotice);
}


void TrayIcon::settings()
{
  ChatCore::i()->startNotify(ChatCore::SettingsNotice);
  ChatCore::i()->startNotify(ChatCore::ShowChatNotice);
}


/*!
 * Обновление состояния в зависимости от статуса пользователя и подключения.
 */
void TrayIcon::update()
{
  int status = m_client->user()->status();
  if (m_client->clientState() != SimpleClient::ClientOnline)
    status = -1;

  if (status == -1) {
    m_icon = SCHAT_ICON(SmallLogoIcon);
  }
  else {
    m_icon = ChatCore::icon(":/images/schat16.png", UserUtils::overlay(status));
  }

  setIcon(m_icon);
}
