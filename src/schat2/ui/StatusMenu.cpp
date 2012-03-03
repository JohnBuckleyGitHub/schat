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

#include "Channel.h"
#include "ChatCore.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/SimpleClient.h"
#include "ui/ChatIcons.h"
#include "ui/StatusMenu.h"

StatusMenu *StatusMenu::m_self = 0;

StatusMenu::StatusMenu(QWidget *parent)
  : QMenu(parent)
{
  m_self = this;

  m_group = new QActionGroup(this);

  addStatus(Status::Online);
  addStatus(Status::Away);
  addStatus(Status::DnD);
  addStatus(Status::FreeForChat);
  addSeparator();
  addStatus(Status::Offline);

  reload();

  connect(ChatCore::settings(), SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));
  connect(ChatClient::io(), SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged()));
  connect(m_group, SIGNAL(triggered(QAction *)), SLOT(statusChanged(QAction *)));
}


QString StatusMenu::statusTitle(int status)
{
  switch (status) {
    case Status::Offline:
      return tr("Offline");

    case Status::Away:
    case Status::AutoAway:
      return tr("Away");

    case Status::DnD:
      return tr("Do not disturb");

    case Status::FreeForChat:
      return tr("Free for Chat");

    default:
      return tr("Online");
  }
}


void StatusMenu::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QMenu::changeEvent(event);
}


void StatusMenu::clientStateChanged()
{
  reload();
}


/*!
 * Обработка изменения настроек статуса или пола.
 */
void StatusMenu::settingsChanged(const QString &key, const QVariant &value)
{
  Q_UNUSED(value)

  if (key == "Profile/Status" || key == "Profile/Gender") {
    reload();
  }
}


void StatusMenu::statusChanged(QAction *action)
{
  applyStatus(action->data().toInt());
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
 * Применение статуса.
 */
void StatusMenu::applyStatus(int status)
{
  ChatClient::channel()->status() = status;
  ChatCore::settings()->setValue("Profile/Status", ChatClient::channel()->status().value());

  if (ChatClient::state() == ChatClient::Online) {
    ChatClient::channels()->update();

    if (ChatClient::channel()->status().value() == Status::Offline)
      ChatClient::io()->leave();

    return;
  }

  if (ChatClient::state() == ChatClient::Offline && ChatClient::channel()->status().value() != Status::Offline)
    ChatClient::open();
}


/*!
 * Обновление меню.
 */
void StatusMenu::reload()
{
  ClientChannel channel(new Channel(ChatClient::id(), ChatClient::channel()->name()));
  channel->setSynced(true);
  channel->gender() = ChatClient::channel()->gender().raw();
  channel->status() = ChatClient::channel()->status().value();

  if (m_statuses.contains(channel->status().value()))
    m_statuses.value(channel->status().value())->setChecked(true);

  if (ChatClient::state() != ChatClient::Online)
    channel->status() = Status::Offline;

  setIcon(ChatIcons::icon(channel));
  setTitle(statusTitle(channel->status().value()));

  QHashIterator<int, QAction *> i(m_statuses);
  while (i.hasNext()) {
    i.next();
    channel->status() = i.key();
    i.value()->setIcon(ChatIcons::icon(channel));
    i.value()->setText(statusTitle(i.key()));
  }

  emit updated();
}
