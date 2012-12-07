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
#include "ChatCore.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientFeeds.h"
#include "hooks/MessagesImpl.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"

ChannelsMenuImpl::ChannelsMenuImpl(QObject *parent)
  : ChannelMenu(parent)
  , m_ignore(0)
  , m_ro(0)
  , m_permissions(0)
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
  else if (action == m_ro) {
    if (action->isChecked())
      ClientFeeds::post(ChatCore::currentId(), LS("acl/head/other/") + SimpleID::encode(action->data().toByteArray()), Acl::Read, Feed::Share | Feed::Broadcast);
    else
      ClientFeeds::del(ChatCore::currentId(), LS("acl/head/other/") + SimpleID::encode(action->data().toByteArray()), Feed::Share | Feed::Broadcast);
  }

  return false;
}


void ChannelsMenuImpl::bindImpl(QMenu *menu, ClientChannel channel, Hooks::Scope scope)
{
  if (channel->type() != SimpleID::UserId)
    return;

  m_self = channel->id() == ChatClient::id();

  if (scope == Hooks::UserViewScope || scope == Hooks::ChatViewScope)
    permissions(menu, channel, scope);

  if (ChatCore::settings()->value(LS("Channels/Ignoring")).toBool() && !m_self) {
    if (!m_permissions)
      menu->addSeparator();

    m_ignore = menu->addAction(SCHAT_ICON(Prohibition), tr("Ignore"));
    m_ignore->setCheckable(true);
    m_ignore->setChecked(Hooks::MessagesImpl::ignored(channel));
    m_ignore->setData(channel->id());
  }
}


void ChannelsMenuImpl::cleanupImpl()
{
  m_ignore = 0;
  m_permissions = 0;
  m_ro = 0;
}


void ChannelsMenuImpl::permissions(QMenu *menu, ClientChannel user, Hooks::Scope scope)
{
  ClientChannel channel = ChatClient::channels()->get(ChatCore::currentId());
  if (!channel)
    return;

  int acl = ClientFeeds::match(channel, ChatClient::channel());
  if (acl == -1 || !(acl & Acl::Edit))
    return;

  menu->addSeparator();
  m_permissions = menu->addMenu(SCHAT_ICON(Key), tr("Permissions"));

  acl = ClientFeeds::match(channel, user);
  m_ro = m_permissions->addAction(tr("Read only"));
  m_ro->setCheckable(true);
  m_ro->setChecked(acl == Acl::Read);
  m_ro->setData(user->id());
}
