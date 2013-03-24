/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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
#include "feeds/AclFeed.h"
#include "hooks/MessagesImpl.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/tabs/ChannelBaseTab.h"
#include "ui/tabs/ChatView.h"
#include "ui/TabWidget.h"

ChannelsMenuImpl::ChannelsMenuImpl(QObject *parent)
  : ChannelMenu(parent)
  , m_advanced(0)
  , m_ignore(0)
  , m_ro(0)
  , m_permissions(0)
{
  add(this);
}


bool ChannelsMenuImpl::triggerImpl(QAction *action)
{
  if (action == m_ignore) {
    ChannelsPluginImpl::ignore(action->data().toByteArray(), action->isChecked());
  }
  else if (action == m_ro) {
    ChannelsPluginImpl::ro(action->data().toByteArray(), ChatCore::currentId(), action->isChecked());
  }
  else if (action == m_advanced) {
    ChannelBaseTab *tab = TabWidget::i()->channelTab(ChatCore::currentId());
    if (tab)
      tab->chatView()->evaluateJavaScript(LS("Channels.editAcl(\"") + SimpleID::encode(action->data().toByteArray()) + LS("\");"));
  }
  else
    return false;

  return true;
}


void ChannelsMenuImpl::bindImpl(QMenu *menu, ClientChannel channel, Hooks::Scope scope)
{
  if (channel->type() != SimpleID::UserId)
    return;

  m_self = channel->id() == ChatClient::id();

  if (scope == Hooks::UserViewScope || scope == Hooks::ChatViewScope)
    permissions(menu, channel);

  if (!m_self)
    invite(menu, channel);

  if (ChatCore::settings()->value(SETTINGS_CHANNELS_IGNORING).toBool() && !m_self) {
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
  m_advanced = 0;
  m_ignore = 0;
  m_permissions = 0;
  m_invite = 0;
  m_ro = 0;
}


void ChannelsMenuImpl::invite(QAction *action)
{
  if (action)
    ChannelsPluginImpl::inviteTo(action->data().toByteArray(), ChatCore::currentId());
}


/*!
 * Формирование меню приглашения в канал.
 */
void ChannelsMenuImpl::invite(QMenu *menu, ClientChannel user)
{
  QList<ClientChannel> list;
  const QMap<QByteArray, ClientChannel>& channels = ChatClient::channels()->channels();
  const QByteArray id                             = ChatClient::id();
  const QByteArray& userId                        = user->id();

  QMapIterator<QByteArray, ClientChannel> i(channels);
  while (i.hasNext()) {
    i.next();

    ClientChannel channel = i.value();
    if (SimpleID::typeOf(i.value()->id()) == SimpleID::ChannelId && channel->channels().contains(id) && !channel->channels().contains(userId) && user->status() != Status::Offline)
      list.append(channel);
  }

  if (list.isEmpty())
    return;

  m_invite = menu->addMenu(ChatIcons::icon(ChatIcons::icon(user, ChatIcons::NoOptions), LS(":/images/add-small.png")), tr("Invite to"));
  foreach (const ClientChannel &channel, list) {
    m_invite->addAction(SCHAT_ICON(Channel), channel->name())->setData(user->id());
  }

  connect(m_invite, SIGNAL(triggered(QAction*)), SLOT(invite(QAction*)));
}


/*!
 * Формирование меню прав доступа к каналу.
 */
void ChannelsMenuImpl::permissions(QMenu *menu, ClientChannel user)
{
  ClientChannel channel = ChatClient::channels()->get(ChatCore::currentId());
  if (!channel || channel->type() != SimpleID::ChannelId)
    return;

  const int acl = ClientFeeds::match(channel, ChatClient::channel());
  if (acl == -1)
    return;

  const bool ro  = !m_self && (acl & Acl::Edit || acl & Acl::SpecialWrite);
  const bool adv = acl & Acl::Edit;

  if (ro || adv) {
    menu->addSeparator();
    m_permissions = menu->addMenu(SCHAT_ICON(Key), tr("Permissions"));

    if (ro) {
      const int acl = ClientFeeds::match(channel, user);
      m_ro = m_permissions->addAction(tr("Read only"));
      m_ro->setCheckable(true);
      m_ro->setChecked(!(acl & Acl::Write) && (acl & Acl::Read));
      m_ro->setData(user->id());
    }

    if (adv) {
      m_permissions->addSeparator();
      m_advanced = m_permissions->addAction(SCHAT_ICON(Gear), tr("Advanced..."));
      m_advanced->setData(user->id());
    }
  }
}
