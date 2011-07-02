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
#include "ui/StatusMenu.h"
#include "ui/UserUtils.h"
#include "User.h"

StatusMenu::StatusMenu(QWidget *parent)
  : QMenu(parent)
{
  m_group = new QActionGroup(this);

  addStatus(User::OnlineStatus);
  addStatus(User::AwayStatus);
  addStatus(User::DnDStatus);
  addStatus(User::FreeForChatStatus);
  addSeparator();
  addStatus(User::OfflineStatus);

  update();
}


void StatusMenu::addStatus(int status)
{
  QAction *action = m_group->addAction("");
  action->setData(status);
  action->setCheckable(true);
  m_statuses.insert(status, action);

  m_group->addAction(action);
  addAction(action);
}


void StatusMenu::update()
{
  ChatUser user(new User(ChatCore::i()->client()->user().data()));
  if (m_statuses.contains(user->status())) {
    m_statuses.value(user->status())->setChecked(true);
  }

  QHashIterator<int, QAction *> i(m_statuses);
  while (i.hasNext()) {
    i.next();
    user->setStatus(i.key());
    i.value()->setIcon(UserUtils::icon(user));
    i.value()->setText(UserUtils::statusTitle(i.key()));
  }
}
