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

#include "ChannelsMenuImpl.h"
#include "ChannelsPlugin_p.h"
#include "client/ChatClient.h"
#include "net/SimpleID.h"
#include "ui/ChatIcons.h"

ChannelsMenuImpl::ChannelsMenuImpl(QObject *parent)
  : ChannelMenu(parent)
  , m_ignore(0)
{
  add(this);
}


bool ChannelsMenuImpl::triggerImpl(QAction *action)
{
  if (action == m_ignore) {
    QByteArray id = action->data().toByteArray();

    if (action->isChecked())
      ChannelsPluginImpl::ignore(id);
    else
      ChannelsPluginImpl::unignore(id);

    return true;
  }

  return false;
}


void ChannelsMenuImpl::bindImpl(QMenu *menu, ClientChannel channel, Hooks::Scope scope)
{
  Q_UNUSED(scope)

  if (channel->type() != SimpleID::UserId || channel->id() == ChatClient::id())
    return;

  menu->addSeparator();
  m_ignore = menu->addAction(SCHAT_ICON(Prohibition), tr("Ignore"));
  m_ignore->setCheckable(true);
  m_ignore->setChecked(ChannelsPluginImpl::ignored(channel));
  m_ignore->setData(channel->id());
  menu->addAction(m_ignore);
}


void ChannelsMenuImpl::cleanupImpl()
{
  m_ignore = 0;
}
