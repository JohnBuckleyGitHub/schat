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

#include <QEvent>

#include "ChatCore.h"
#include "client/SimpleClient.h"
#include "NetworkManager.h"
#include "ui/network/LoginWidget.h"
#include "ui/network/NetworkTabs.h"
#include "ui/network/SignUpWidget.h"

NetworkTabs::NetworkTabs(QWidget *parent)
  : QTabWidget(parent)
{
  m_login = new LoginWidget(this);

  setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Maximum);
  addTab(m_login, tr("Log In"));

  reload();
  retranslateUi();

  connect(this, SIGNAL(currentChanged(int)), SLOT(indexChanged(int)));
}


/*!
 * \sa LoginWidget::canLogIn().
 */
bool NetworkTabs::canLogIn() const
{
  return m_login->canLogIn();
}


/*!
 * Обновление состояния виджета.
 */
void NetworkTabs::reload()
{
  m_login->reload();

  if (SignUpWidget::canSignUp()) {
    if (!m_signup) {
      m_signup = new SignUpWidget(this);
      addTab(m_signup, tr("Sign Up"));
    }
  }
  else {
    if (m_signup) {
      removeTab(indexOf(m_signup));
      delete m_signup;
    }
  }

  if (m_signup)
    m_signup->reload();
}


void NetworkTabs::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void NetworkTabs::indexChanged(int index)
{
  if (m_signup)
    m_signup->setSmall(indexOf(m_signup) != index);

  m_login->adjustSize();
  adjustSize();
}


void NetworkTabs::retranslateUi()
{
  m_login->retranslateUi();

  if (m_signup)
    m_signup->retranslateUi();
}
