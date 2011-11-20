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
#include <QVBoxLayout>
#include <QTextDocument>

#include "actions/UserMenu.h"
#include "ChatCore.h"
#include "ChatPlugins.h"
#include "client/SimpleClient.h"
#include "net/packets/message.h"
#include "ui/tabs/ChatView.h"
#include "ui/tabs/PrivateTab.h"
#include "ui/TabWidget.h"
#include "ui/UserUtils.h"

PrivateTab::PrivateTab(ClientUser user, TabWidget *parent)
  : ChatViewTab(QLatin1String("qrc:/html/ChatView.html"), user->id(), PrivateType, parent)
  , m_user(user)
{
  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_chatView);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  setIcon(userIcon());
  setText(m_user->nick());

  MessageData data(UserUtils::userId(), SimpleID::setType(SimpleID::ChannelId, user->id()), QLatin1String("join"), QLatin1String("~") + user->nick());
  ChatCore::i()->client()->send(data);

  PrivateTabHook hook(this);
  ChatCore::i()->plugins()->hook(hook);

  connect(ChatCore::i()->client(), SIGNAL(userLeave(const QByteArray &)), SLOT(userLeave(const QByteArray &)));
  connect(ChatCore::i()->client(), SIGNAL(split(const QList<QByteArray> &)), SLOT(split(const QList<QByteArray> &)));
  connect(ChatCore::i()->client(), SIGNAL(userDataChanged(const QByteArray &, int)), SLOT(userDataChanged(const QByteArray &, int)));
}


PrivateTab::~PrivateTab()
{
  ChatCore::i()->client()->part(SimpleID::setType(SimpleID::ChannelId, m_user->id()));
}


bool PrivateTab::bindMenu(QMenu *menu)
{
  UserMenu *builder = new UserMenu(m_user, this);
  builder->bind(menu);
  return true;
}


bool PrivateTab::update(ClientUser user)
{
  if (!user)
    return false;

  if (m_user != user) {
    addJoinMsg(user->id(), user->id());
    m_user = user;
  }

  m_action->setText(m_user->nick());
  m_icon = userIcon();

  int index = m_tabs->indexOf(this);
  if (index == -1)
    return false;

  m_tabs->setTabText(index, m_user->nick());
  setOnline(true);
  return true;
}


void PrivateTab::alert(bool start)
{
  ChatViewTab::alert(start);

  if (m_alerts > 1)
    return;

  setIcon(userIcon());
}


void PrivateTab::setOnline(bool online)
{
  if (!online)
    m_user->setStatus(Status::Offline);

  m_tabs->setTabToolTip(m_tabs->indexOf(this), UserUtils::toolTip(m_user));
  AbstractTab::setOnline(online);
}


void PrivateTab::split(const QList<QByteArray> &users)
{
  if (users.contains(id()))
    userLeave(id());
}


void PrivateTab::userDataChanged(const QByteArray &userId, int changed)
{
  if (!(changed & SimpleClient::UserNickChanged))
    return;

//  ClientUser user;
//  if (UserUtils::userId() == userId)
//    user = UserUtils::user();
//  else if (id() == userId)
//    user = UserUtils::user(userId);
//
//  if (!user)
//    return;
//
//  UserUtils::updateUserNick(m_chatView, user);
}


void PrivateTab::userLeave(const QByteArray &userId)
{
  if (id() == userId) {
    addQuitMsg(userId, userId);
    setOnline(false);
  }
}


QIcon PrivateTab::userIcon() const
{
  if (m_alerts)
    return ChatCore::icon(UserUtils::icon(m_user, false, true), QLatin1String(":/images/message-small.png"));
  else
    return UserUtils::icon(m_user, true, true);
}

