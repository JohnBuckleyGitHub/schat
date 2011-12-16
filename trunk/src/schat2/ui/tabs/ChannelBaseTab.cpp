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

#include <QDebug>

#include <QApplication>
#include <QFile>

#include <QTextDocument>

#include "ChatUrls.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "net/SimpleID.h"
#include "SimpleJSon.h"
#include "ui/ChatIcons.h"
#include "ui/tabs/ChannelBaseTab.h"
#include "ui/tabs/ChatView.h"

ChannelBaseTab::ChannelBaseTab(ClientChannel channel, TabType type, TabWidget *parent)
  : AbstractTab(channel->id(), type, parent)
  , m_channel(channel)
  , m_alerts(0)
{
  QString file = QApplication::applicationDirPath() + "/styles/test/html/ChatView.html";
  if (QFile::exists(file))
    file = QUrl::fromLocalFile(file).toString();
  else
    file = "qrc:/html/ChatView.html";

  m_chatView = new ChatView(channel->id(), file, this);
  setIcon(channelIcon());

  connect(ChatClient::channels(), SIGNAL(channel(const ChannelInfo &)), SLOT(channel(const ChannelInfo &)));
  connect(ChatClient::i(), SIGNAL(offline()), SLOT(offline()));
}


ChannelBaseTab::~ChannelBaseTab()
{
  ChatClient::channels()->part(id());
}


void ChannelBaseTab::alert(bool start)
{
  if (start)
    m_alerts++;
  else
    m_alerts = 0;

  if (m_alerts > 1)
    return;

  setIcon(channelIcon());
}


void ChannelBaseTab::add(const Message &message)
{
  m_chatView->add(message);
}


void ChannelBaseTab::setOnline(bool online)
{
  if (online) {
    AbstractTab::setOnline(online);
    return;
  }

  m_online = false;
  setIcon(channelIcon());
}


void ChannelBaseTab::channel(const ChannelInfo &info)
{
  if (SimpleID::typeOf(info.id()) == SimpleID::UserId && info.option() != ChannelInfo::Updated)
    rename(info.id());

  if (info.id() != id())
    return;

  reload();

  if (!m_online)
    m_online = true;
}


void ChannelBaseTab::offline()
{
  setOnline(false);
}


QIcon ChannelBaseTab::channelIcon() const
{
  if (m_alerts)
    return ChatIcons::icon(ChatIcons::icon(m_channel, ChatIcons::OfflineStatus), ":/images/message-small.png");
  else
    return ChatIcons::icon(m_channel);
}


void ChannelBaseTab::reload()
{
  setIcon(channelIcon());
  setText(m_channel->name());
}


void ChannelBaseTab::rename(const QByteArray &id)
{
  ClientChannel user = ChatClient::channels()->get(id);

  if (!user)
    return;

  QVariantMap json;
  json["Id"]   = SimpleID::encode(user->id());
  json["Url"]  = ChatUrls::toUrl(user, "insert").toString();
  json["Name"] = Qt::escape(user->name());

  m_chatView->evaluateJavaScript("updateChannelName(" + SimpleJSon::quote(SimpleJSon::generate(json)) + ");");
}
