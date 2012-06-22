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

#include <QEvent>
#include <QMenu>

#include "Account.h"
#include "ChatNotify.h"
#include "client/ChatClient.h"
#include "hooks/RegCmds.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/network/AccountButton.h"
#include "ui/network/LoginWidget.h"
#include "ui/network/NetworkWidget.h"
#include "ui/network/Password.h"
#include "ui/network/SignUpWidget.h"

AccountButton::AccountButton(NetworkWidget *parent)
  : QToolButton(parent)
  , m_network(parent)
{
  m_menu = new QMenu(this);

  m_signIn = m_menu->addAction(SCHAT_ICON(SignIn), tr("Sign in"));
  m_signOut = m_menu->addAction(SCHAT_ICON(SignOut), tr("Sign out"));
  m_signUp = m_menu->addAction(SCHAT_ICON(SignUp), tr("Sign up"));
  m_menu->addSeparator();
  m_reset = m_menu->addAction(SCHAT_ICON(Password), tr("Forgot password?"));
  m_password = m_menu->addAction(SCHAT_ICON(Password), tr("Change password"));
  m_computers = m_menu->addAction(SCHAT_ICON(Computer), tr("My Computers"));

  setIcon(SCHAT_ICON(Key));
  setMenu(m_menu);
  setPopupMode(QToolButton::InstantPopup);
  setToolTip(tr("Account"));
  setEnabled(ChatClient::state() == ChatClient::Online);

  connect(m_menu, SIGNAL(triggered(QAction *)), SLOT(menuTriggered(QAction *)));
  connect(m_menu, SIGNAL(aboutToShow()), SLOT(showMenu()));
}


void AccountButton::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QToolButton::changeEvent(event);
}


void AccountButton::menuTriggered(QAction *action)
{
  if (action == m_computers) {
    ChatNotify::start(Notify::OpenInfo, ChatClient::serverId());
  }
  else if (action == m_password) {
    m_network->add(new Password(this));
  }
  else if (action == m_reset) {
    m_network->add(new SignUpWidget(this, LS("reset")));
  }
  else if (action == m_signIn) {
    m_network->add(new LoginWidget(this));
  }
  else if (action == m_signOut) {
    RegCmds::signOut();
  }
  else if (action == m_signUp) {
    m_network->add(new SignUpWidget(this));
  }
}


void AccountButton::showMenu()
{
  bool account = !ChatClient::channel()->account()->name().isEmpty();
  m_signIn->setVisible(!account);
  m_signOut->setVisible(account);
  m_signUp->setVisible(!account);
  m_reset->setVisible(!account);
  m_password->setVisible(account);
}


void AccountButton::retranslateUi()
{
  m_signIn->setText(tr("Sign in"));
  m_signOut->setText(tr("Sign out"));
  m_signUp->setText(tr("Sign up"));
  m_reset->setText(tr("Forgot password?"));
  m_password->setText(tr("Change password"));
  m_computers->setText(tr("My Computers"));
  setToolTip(tr("Account"));
}
