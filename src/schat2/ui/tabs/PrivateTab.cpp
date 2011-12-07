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

#include <QVBoxLayout>

#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "ui/ChannelUtils.h"
#include "ui/tabs/ChatView.h"
#include "ui/tabs/PrivateTab.h"
#include "ui/TabWidget.h"

PrivateTab::PrivateTab(ClientChannel channel, TabWidget *parent)
  : ChannelBaseTab(channel, PrivateType, parent)
{
  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_chatView);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  setIcon(userIcon());
  setText(m_channel->name());

  ChatClient::channels()->join(id());
}


bool PrivateTab::bindMenu(QMenu *menu)
{
//  UserMenu *builder = new UserMenu(m_user, this);
//  builder->bind(menu);
  return true;
}


//bool PrivateTab::update(ClientUser user)
//{
//  if (!user)
//    return false;
//
//  if (m_user != user) {
//    addJoinMsg(user->id(), user->id());
//    m_user = user;
//  }
//
//  m_action->setText(m_user->nick());
//  m_icon = userIcon();
//
//  int index = m_tabs->indexOf(this);
//  if (index == -1)
//    return false;
//
//  m_tabs->setTabText(index, m_user->nick());
//  setOnline(true);
//  return true;
//}


QIcon PrivateTab::userIcon() const
{
//  if (m_alerts)
//    return ChatCore::icon(UserUtils::icon(m_user, false, true), QLatin1String(":/images/message-small.png"));
//  else
//    return UserUtils::icon(m_user, true, true);
  return ChannelUtils::icon(m_channel, ChannelUtils::Statuses | ChannelUtils::OfflineStatus);
}
