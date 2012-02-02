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

#include <QMenu>

#include "Account.h"
#include "client/ChatClient.h"
#include "ui/ChatIcons.h"
#include "ui/network/AccountButton.h"

AccountButton::AccountButton(QWidget *parent)
  : QToolButton(parent)
{
  m_menu = new QMenu(this);

  m_signIn = m_menu->addAction(SCHAT_ICON(SignIn), tr("Sign in"));
  m_signOut = m_menu->addAction(SCHAT_ICON(SignOut), tr("Sign out"));
  m_signUp = m_menu->addAction(SCHAT_ICON(SignUp), tr("Sign up"));
  m_menu->addSeparator();
  m_reset = m_menu->addAction(SCHAT_ICON(Password), tr("Forgot password?"));
  m_password = m_menu->addAction(SCHAT_ICON(Password), tr("Change password"));

  setIcon(SCHAT_ICON(Key));
  setMenu(m_menu);
  setPopupMode(QToolButton::InstantPopup);
  setToolTip(tr("Account"));
  setEnabled(ChatClient::state() == ChatClient::Online);

  connect(m_menu, SIGNAL(triggered(QAction *)), SLOT(menuTriggered(QAction *)));
  connect(m_menu, SIGNAL(aboutToShow()), SLOT(showMenu()));
}


void AccountButton::menuTriggered(QAction *action)
{

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
