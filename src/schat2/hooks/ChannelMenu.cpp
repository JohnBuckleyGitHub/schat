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
#include "ChatUrls.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "hooks/ChannelMenu.h"
#include "ui/ChatIcons.h"

namespace Hooks
{

ChannelMenu *ChannelMenu::m_self = 0;

ChannelMenu::ChannelMenu(QObject *parent)
  : QObject(parent)
{
  if (!m_self)
    m_self = this;
}


ChannelMenu::~ChannelMenu()
{
  if (m_self == this)
    m_self = 0;
}


void ChannelMenu::bind(QMenu *menu, ClientChannel channel, Scope scope)
{
  if (!channel || m_self->m_hooks.isEmpty())
    return;

  connect(menu, SIGNAL(triggered(QAction*)), m_self, SLOT(triggered(QAction*)));
  connect(menu, SIGNAL(destroyed(QObject*)), m_self, SLOT(cleanup()));

  foreach (ChannelMenu *hook, m_self->m_hooks) {
    hook->bindImpl(menu, channel, scope);
  }
}


void ChannelMenu::cleanup()
{
  if (m_hooks.isEmpty())
    return;

  foreach (ChannelMenu *hook, m_hooks) {
    hook->cleanupImpl();
  }
}


void ChannelMenu::triggered(QAction *action)
{
  if (action->data().type() == QVariant::Url) {
    ChatUrls::open(action->data().toUrl());
    return;
  }

  foreach (ChannelMenu *hook, m_hooks) {
    if (hook->triggerImpl(action))
      return;
  }
}


/*!
 * Обработка выбора действия.
 */
bool ChannelMenu::triggerImpl(QAction *action)
{
  Q_UNUSED(action);
  return false;
}


/*
 * Создание меню.
 */
void ChannelMenu::bindImpl(QMenu *menu, ClientChannel channel, Scope scope)
{
  Q_UNUSED(menu)
  Q_UNUSED(channel)
  Q_UNUSED(scope)
}


void ChannelMenu::cleanupImpl()
{
}

} // namespace Hooks
