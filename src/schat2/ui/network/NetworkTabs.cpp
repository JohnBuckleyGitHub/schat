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
  m_signup = new SignUpWidget(this);
  setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Maximum);

  addTab(m_login, tr("Log In"));
//  addTab(m_signup, tr("Sign Up"));

  connect(ChatCore::i(), SIGNAL(notify(int, const QVariant &)), SLOT(notify(int, const QVariant &)));
  connect(ChatCore::i()->client(), SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int)));

  update(ChatCore::i()->networks()->serverId());
  retranslateUi();
}


bool NetworkTabs::canSignUp(const QByteArray &id) const
{
  if (id.isEmpty())
    return false;

  if (!ChatCore::i()->networks()->isItem(id))
    return false;

  if (ChatCore::i()->client()->clientState() != SimpleClient::ClientOnline)
    return false;

  if (ChatCore::i()->client()->serverId() != id)
    return false;

  return true;
}


void NetworkTabs::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void NetworkTabs::clientStateChanged(int state)
{
  update(ChatCore::i()->networks()->serverId());
}


void NetworkTabs::notify(int notice, const QVariant &data)
{
  if (notice == ChatCore::NetworkSelectedNotice || notice == ChatCore::NetworkChangedNotice) {
    if (data.type() == QVariant::ByteArray)
      update(data.toByteArray());
  }
}


void NetworkTabs::retranslateUi()
{
  m_login->retranslateUi();
}


void NetworkTabs::update(const QByteArray &id)
{
  if (!ChatCore::i()->networks()->isItem(id))
    return;

  m_login->update(id);
  updateSignUp(id);
}


void NetworkTabs::updateSignUp(const QByteArray &id)
{
  int index = indexOf(m_signup);
  if (canSignUp(id) && index == -1)
    addTab(m_signup, tr("Sign Up"));
  else if (index != -1)
    removeTab(index);
}
