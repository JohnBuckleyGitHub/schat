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
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/SimpleClient.h"
#include "hooks/ChannelMenu.h"
#include "messages/ServiceMessage.h"
#include "ui/InputWidget.h"
#include "ui/tabs/ChannelBar.h"
#include "ui/tabs/ChannelTab.h"
#include "ui/tabs/ChatView.h"
#include "ui/tabs/UserView.h"
#include "ui/TabWidget.h"

ChannelTab::ChannelTab(ClientChannel channel, TabWidget *parent)
  : ChannelBaseTab(channel, ChannelType, parent)
{
  m_bar = new ChannelBar(this);
  m_bar->setVisible(false);

  m_userView = new UserView(this);

  m_leftLayout = new QVBoxLayout(this);
  m_leftLayout->addWidget(m_bar);
  m_leftLayout->addWidget(m_chatView);
  m_leftLayout->setMargin(0);
  m_leftLayout->setSpacing(0);

  QWidget *left = new QWidget(this);
  left->setLayout(m_leftLayout);

  m_splitter = new QSplitter(this);
  m_splitter->addWidget(left);
  m_splitter->addWidget(m_userView);
  m_splitter->setStretchFactor(0, 1);
  m_splitter->setStretchFactor(1, 1);

  m_userCount = SCHAT_OPTION("ChannelUserCount").toBool();

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_splitter);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  setText(channel->name());

  connect(ChatCore::settings(), SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));
  connect(ChatClient::channels(), SIGNAL(channels(const QList<QByteArray> &)), SLOT(channels(const QList<QByteArray> &)));
  connect(ChatClient::channels(), SIGNAL(joined(const QByteArray &, const QByteArray &)), SLOT(joined(const QByteArray &, const QByteArray &)));
  connect(ChatClient::channels(), SIGNAL(part(const QByteArray &, const QByteArray &)), SLOT(part(const QByteArray &, const QByteArray &)));
  connect(ChatClient::channels(), SIGNAL(quit(const QByteArray &)), SLOT(quit(const QByteArray &)));
  connect(ChatClient::channels(), SIGNAL(channel(const QByteArray &)), SLOT(channel(const QByteArray &)));

  m_chatView->add(ServiceMessage::joined(ChatClient::id()));
}


void ChannelTab::setOnline(bool online)
{
  if (!online) {
    m_userView->clear();
    displayUserCount();
  }

  ChannelBaseTab::setOnline(online);
}


void ChannelTab::channel(const QByteArray &id)
{
  if (this->id() != id)
    return;

  m_chatView->add(ServiceMessage::joined(ChatClient::id()));
}


/*!
 * Обработка получения данных о новых каналах.
 * Если канал содержит пользователя, но ещё не был добавлен в список пользователей, то он будет добавлен.
 *
 * \param channels Список идентификаторов каналов.
 */
void ChannelTab::channels(const QList<QByteArray> &channels)
{
  foreach (QByteArray id, channels) {
    if (m_channel->channels().all().contains(id) && !m_userView->contains(id))
      m_userView->add(ChatClient::channels()->get(id));
  }
}


/*!
 * Обработка входа пользователя в канал.
 */
void ChannelTab::joined(const QByteArray &channel, const QByteArray &user)
{
  if (id() != channel)
    return;

  m_userView->add(ChatClient::channels()->get(user));

  if (ChatClient::id() != user)
    m_chatView->add(ServiceMessage::joined(user));
}


void ChannelTab::part(const QByteArray &channel, const QByteArray &user)
{
  if (id() != channel)
    return;

  m_userView->remove(user);
  m_chatView->add(ServiceMessage::part(user));
}


void ChannelTab::quit(const QByteArray &user)
{
  if (!m_channel->channels().all().contains(user))
    return;

  m_userView->remove(user);
  m_chatView->add(ServiceMessage::quit(user));
}


void ChannelTab::settingsChanged(const QString &key, const QVariant &value)
{
  if (key == QLatin1String("ChannelUserCount")) {
    m_userCount = value.toBool();
    displayUserCount();
  }
}


void ChannelTab::displayUserCount()
{
//  int index = m_tabs->indexOf(this);
//  if (index == -1)
//    return;
//
//  if (m_userCount && m_userView->userCount() > 1)
//    m_tabs->setTabText(index, QString("%1 (%2)").arg(m_channel->name()).arg(m_userView->userCount()));
//  else
//    m_tabs->setTabText(index, m_channel->name());
}
