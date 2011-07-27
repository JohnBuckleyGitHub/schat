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
#include "ChatSettings.h"
#include "client/SimpleClient.h"
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

  connect(ChatCore::i()->settings(), SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));
  connect(ChatCore::i()->client(), SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int)));
  connect(m_group, SIGNAL(triggered(QAction *)), SLOT(statusChanged(QAction *)));
}


void StatusMenu::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QMenu::changeEvent(event);
}


void StatusMenu::clientStateChanged(int state)
{
  Q_UNUSED(state);
  update();
}


/*!
 * Обработка изменения настроек статуса или пола.
 */
void StatusMenu::settingsChanged(const QString &key, const QVariant &value)
{
  Q_UNUSED(value)

  if (key == QLatin1String("Profile/Status") || key == QLatin1String("Profile/Gender")) {
    update();
  }
}


void StatusMenu::statusChanged(QAction *action)
{
  ChatCore::i()->settings()->updateValue(QLatin1String("Profile/Status"), action->data().toInt());
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


/*!
 * Обновление меню.
 */
void StatusMenu::update()
{
  ClientUser user(new User(ChatCore::i()->client()->user().data()));
  if (m_statuses.contains(user->status())) {
    m_statuses.value(user->status())->setChecked(true);
  }

  if (ChatCore::i()->client()->clientState() != SimpleClient::ClientOnline) {
    user->setStatus(User::OfflineStatus);
  }

  setIcon(UserUtils::icon(user, true, true));
  setTitle(UserUtils::statusTitle(user->status()));

  QHashIterator<int, QAction *> i(m_statuses);
  while (i.hasNext()) {
    i.next();
    user->setStatus(i.key());
    i.value()->setIcon(UserUtils::icon(user, true, true));
    i.value()->setText(UserUtils::statusTitle(i.key()));
  }

  emit updated();
}
