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

#include <QAction>
#include <QVBoxLayout>

#include "actions/UserMenu.h"
#include "ChatCore.h"
#include "ui/tabs/ChatView.h"
#include "ui/tabs/PrivateTab.h"
#include "ui/TabWidget.h"
#include "ui/UserUtils.h"

PrivateTab::PrivateTab(ClientUser user, TabWidget *parent)
  : ChatViewTab("qrc:/html/ChatView.html", user->id(), PrivateType, parent)
  , m_user(user)
{
  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_chatView);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  setIcon(UserUtils::icon(user));
  setText(m_user->nick());
}


bool PrivateTab::update(ClientUser user)
{
  if (!user)
    return false;

  if (m_user != user)
    m_user = user;

  m_action->setText(m_user->nick());
  m_icon = userIcon();

  int index = m_tabs->indexOf(this);
  if (index == -1)
    return false;

  m_tabs->setTabText(index, m_user->nick());
  setOnline(true);
  return true;
}


MenuBuilder *PrivateTab::menu()
{
  return new UserMenu(m_user, this);
}


void PrivateTab::alert(bool start)
{
  ChatViewTab::alert(start);

  if (m_alerts > 1)
    return;

  setIcon(userIcon());
}


void PrivateTab::setOnline(bool online)
{
  m_tabs->setTabToolTip(m_tabs->indexOf(this), UserUtils::toolTip(m_user));
  AbstractTab::setOnline(online);
}


QIcon PrivateTab::userIcon() const
{
  if (m_alerts)
    return ChatCore::icon(UserUtils::icon(m_user, false), QLatin1String(":/images/message-small.png"));
  else
    return UserUtils::icon(m_user, true, true);
}

