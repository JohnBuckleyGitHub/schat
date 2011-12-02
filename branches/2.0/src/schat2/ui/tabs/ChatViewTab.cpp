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

#include "ui/tabs/ChatView.h"
#include "ui/tabs/ChatViewTab.h"
#include "ui/TabWidget.h"
#include "ui/UserUtils.h"
#include "Channel.h"

ChatViewTab::ChatViewTab(const QString &url, const QByteArray &id, TabType type, TabWidget *parent)
  : AbstractTab(id, type, parent)
  , m_alerts(0)
{
  Q_UNUSED(url)

  m_chatView = new ChatView(id, url, this);
}


void ChatViewTab::alert(bool start)
{
  if (start)
    m_alerts++;
  else
    m_alerts = 0;
}


void ChatViewTab::addJoinMsg(const QByteArray &userId, const QByteArray &destId)
{
  ClientUser user = UserUtils::user(userId);
  if (!user)
    return;

  if (user->status() == Status::Offline)
    return;

  QString text;
  if (user->gender().value() == Gender::Female)
    text = tr("has joined", "Female");
  else
    text = tr("has joined", "Male");

//  m_tabs->addServiceMsg(userId, destId, text, this);
}


void ChatViewTab::addLeftMsg(const QByteArray &userId, const QByteArray &destId)
{
  ClientUser user = UserUtils::user(userId);
  if (!user)
    return;

  QString text;
  if (user->gender().value() == Gender::Female)
    text = tr("has left", "Female");
  else
    text = tr("has left", "Male");

//  m_tabs->addServiceMsg(userId, destId, text, this);
}


void ChatViewTab::addQuitMsg(const QByteArray &userId, const QByteArray &destId)
{
  ClientUser user = UserUtils::user(userId);
  if (!user)
    return;

  QString text;
  if (user->gender().value() == Gender::Female)
    text = tr("has quit chat", "Female");
  else
    text = tr("has quit chat", "Male");

//  m_tabs->addServiceMsg(userId, destId, text, this);
}
