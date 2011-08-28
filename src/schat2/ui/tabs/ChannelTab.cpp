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
#include "ChatSettings.h"
#include "client/SimpleClient.h"
#include "ui/tabs/ChannelTab.h"
#include "ui/tabs/ChatView.h"
#include "ui/tabs/UserView.h"
#include "ui/TabWidget.h"
#include "ui/UserUtils.h"

ChannelTab::ChannelTab(ClientChannel channel, TabWidget *parent)
  : ChatViewTab(QLatin1String("qrc:/html/ChatView.html"), channel->id(), ChannelType, parent)
  , m_channel(channel)
  , m_client(ChatCore::i()->client())
{
  m_userView = new UserView(this);

  m_splitter = new QSplitter(this);
  m_splitter->addWidget(m_chatView);
  m_splitter->addWidget(m_userView);
  m_splitter->setStretchFactor(0, 1);
  m_splitter->setStretchFactor(1, 1);

  m_userCount = SCHAT_OPTION("ChannelUserCount").toBool();

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_splitter);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  setIcon(SCHAT_ICON(ChannelIcon));
  setText(channel->name());

  connect(m_client, SIGNAL(userLeave(const QByteArray &)), SLOT(userLeave(const QByteArray &)));
  connect(m_client, SIGNAL(split(const QList<QByteArray> &)), SLOT(split(const QList<QByteArray> &)));
  connect(m_client, SIGNAL(part(const QByteArray &, const QByteArray &)), SLOT(part(const QByteArray &, const QByteArray &)));
  connect(ChatCore::i()->settings(), SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));
}


ChannelTab::~ChannelTab()
{
  ChatCore::i()->client()->part(m_channel->id());
}


bool ChannelTab::add(ClientUser user)
{
  if (!m_userView->add(user))
    return false;

  if (m_userView->isSortable() || UserUtils::userId() == user->id()) {
    addJoinMsg(user->id(), m_channel->id());
    displayUserCount();
  }

  return true;
}


bool ChannelTab::remove(const QByteArray &id)
{
  if (!m_userView->remove(id))
    return false;

  addLeftMsg(id, m_channel->id());
  displayUserCount();
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
    displayUserCount();

    if (ChatCore::i()->client()->previousState() == SimpleClient::ClientOnline)
      addQuitMsg(UserUtils::userId(), m_channel->id());
  }
  else {
    ClientChannel channel = ChatCore::i()->client()->channel(id());
    if (channel && channel != m_channel)
      m_channel = channel;
  }

  AbstractTab::setOnline(online);
}


void ChannelTab::synced()
{
  displayUserCount();
  m_userView->sort();
}


void ChannelTab::part(const QByteArray &channelId, const QByteArray &userId)
{
  if (id() != channelId)
    return;

  ClientUser user = m_client->user(userId);
  if (user)
    remove(userId);
}


void ChannelTab::settingsChanged(const QString &key, const QVariant &value)
{
  if (key == QLatin1String("ChannelUserCount")) {
    m_userCount = value.toBool();
    displayUserCount();
  }
}


void ChannelTab::split(const QList<QByteArray> &users)
{
  for (int i = 0; i < users.size(); ++i) {
    m_userView->remove(users.at(i));
  }
}


void ChannelTab::userLeave(const QByteArray &userId)
{
  if (m_userView->remove(userId)) {
    addQuitMsg(userId, m_channel->id());
    displayUserCount();
  }
}


void ChannelTab::displayUserCount()
{
  int index = m_tabs->indexOf(this);
  if (index == -1)
    return;

  if (m_userCount && m_userView->userCount() > 1)
    m_tabs->setTabText(index, QString("%1 (%2)").arg(m_channel->name()).arg(m_userView->userCount()));
  else
    m_tabs->setTabText(index, m_channel->name());
}
