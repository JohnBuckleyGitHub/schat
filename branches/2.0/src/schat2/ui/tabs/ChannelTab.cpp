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
#include <QSplitter>
#include <QVBoxLayout>

#include "ChatCore.h"
#include "client/SimpleClient.h"
#include "ui/tabs/ChannelTab.h"
#include "ui/tabs/ChatView.h"
#include "ui/tabs/UserView.h"
#include "ui/UserUtils.h"

ChannelTab::ChannelTab(ClientChannel channel, TabWidget *parent)
  : ChatViewTab(QLatin1String("qrc:/html/ChatView.html"), channel->id(), ChannelType, parent)
  , m_channel(channel)
{
  m_userView = new UserView(this);

  m_splitter = new QSplitter(this);
  m_splitter->addWidget(m_chatView);
  m_splitter->addWidget(m_userView);
  m_splitter->setStretchFactor(0, 1);
  m_splitter->setStretchFactor(1, 1);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_splitter);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  setIcon(SCHAT_ICON(ChannelIcon));
  setText(channel->name());

  connect(ChatCore::i()->client(), SIGNAL(userLeave(const QByteArray &)), SLOT(userLeave(const QByteArray &)));
}


ChannelTab::~ChannelTab()
{
  ChatCore::i()->client()->part(m_channel->id());
}


bool ChannelTab::add(ClientUser user)
{
  if (!m_userView->add(user))
    return false;

  if (m_userView->isSortable() || UserUtils::userId() == user->id())
    addJoinMsg(user->id(), m_channel->id());

  return true;
}


bool ChannelTab::remove(const QByteArray &id)
{
  if (!m_userView->remove(id))
    return false;

  addLeftMsg(id, m_channel->id());
  return true;
}


void ChannelTab::alert(bool start)
{
  ChatViewTab::alert(start);

  if (m_alerts > 1)
    return;

  if (start)
    setIcon(SCHAT_ICON(ChannelAlertIcon));
  else
    setIcon(SCHAT_ICON(ChannelIcon));
}


void ChannelTab::setOnline(bool online)
{
  if (!online) {
    m_userView->clear();
    if (ChatCore::i()->client()->previousState() == SimpleClient::ClientOnline)
      addQuitMsg(UserUtils::userId(), m_channel->id());
  }

  AbstractTab::setOnline(online);
}


void ChannelTab::userLeave(const QByteArray &userId)
{
  if (m_userView->remove(userId))
    addQuitMsg(userId, m_channel->id());
}
