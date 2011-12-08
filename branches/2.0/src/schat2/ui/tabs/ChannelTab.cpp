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

#include "actions/ChannelMenu.h"
#include "ChatCore.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/SimpleClient.h"
#include "messages/TopicMessage.h"
#include "net/packets/messages.h"
#include "ui/InputWidget.h"
#include "ui/tabs/ChannelBar.h"
#include "ui/tabs/ChannelTab.h"
#include "ui/tabs/ChatView.h"
#include "ui/tabs/UserView.h"
#include "ui/TabWidget.h"
#include "ui/UserUtils.h"

ChannelTab::ChannelTab(ClientChannel channel, TabWidget *parent)
  : ChannelBaseTab(channel, ChannelType, parent)
  , m_client(ChatCore::i()->client())
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

  setIcon(SCHAT_ICON(ChannelIcon));
  setText(channel->name());

//  TopicMessage msg(m_channel->topic());
//  m_tabs->message(this, msg);

  connect(m_client, SIGNAL(userLeave(const QByteArray &)), SLOT(userLeave(const QByteArray &)));
  connect(m_client, SIGNAL(split(const QList<QByteArray> &)), SLOT(split(const QList<QByteArray> &)));
  connect(m_client, SIGNAL(userDataChanged(const QByteArray &, int)), SLOT(userDataChanged(const QByteArray &, int)));
  connect(ChatCore::i()->settings(), SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));
  connect(ChatCore::i(), SIGNAL(notify(int, const QVariant &)), SLOT(notify(int, const QVariant &)));
  connect(m_bar->topic(), SIGNAL(send(const QString &)), SLOT(sendTopic(const QString &)));
  connect(m_bar->topic(), SIGNAL(focusOut()), SLOT(topicFocusOut()));
  connect(m_chatView, SIGNAL(reloaded()), SLOT(reloaded()));

  connect(ChatClient::channels(), SIGNAL(channels(const QList<QByteArray> &)), SLOT(channels(const QList<QByteArray> &)));
  connect(ChatClient::channels(), SIGNAL(joined(const QByteArray &, const QByteArray &)), SLOT(joined(const QByteArray &, const QByteArray &)));
  connect(ChatClient::channels(), SIGNAL(part(const QByteArray &, const QByteArray &)), SLOT(part(const QByteArray &, const QByteArray &)));
  connect(ChatClient::channels(), SIGNAL(quit(const QByteArray &)), SLOT(quit(const QByteArray &)));
}


bool ChannelTab::bindMenu(QMenu *menu)
{
  ChannelMenu *builder = new ChannelMenu(m_channel, this);
  builder->bind(menu);
  return true;
}


bool ChannelTab::remove(const QByteArray &id)
{
  if (!m_userView->remove(id))
    return false;

//  addLeftMsg(id, m_channel->id());
  displayUserCount();
  return true;
}


void ChannelTab::alert(bool start)
{
  ChannelBaseTab::alert(start);

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

//    if (ChatCore::i()->client()->previousState() == SimpleClient::ClientOnline)
//      addQuitMsg(UserUtils::userId(), m_channel->id());
  }
  else {
    ClientChannel channel = ChatClient::channels()->get(id());
    if (channel && channel != m_channel)
      m_channel = channel;
  }

  ChannelBaseTab::setOnline(online);
}


void ChannelTab::synced()
{
  displayUserCount();
  m_userView->sort();
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
}


void ChannelTab::notify(int notice, const QVariant &data)
{
  if (notice == ChatCore::EditTopicNotice) {
    if (data.toByteArray() != id())
      return;

    m_chatView->evaluateJavaScript("showTopic", false);
    m_tabs->setCurrentWidget(this);
    m_bar->topic()->clear();
//    m_bar->topic()->insertHtml(m_channel->topic().topic + " ");
    m_bar->setVisible(true);
    m_bar->topic()->adjustHeight();
    m_bar->topic()->setFocus();
  }
}


void ChannelTab::part(const QByteArray &channel, const QByteArray &user)
{
  if (id() != channel)
    return;

  m_userView->remove(user);
}


void ChannelTab::quit(const QByteArray &user)
{
  if (!m_channel->channels().all().contains(user))
    return;

  m_userView->remove(user);
}


void ChannelTab::reloaded()
{
//  TopicMessage msg(m_channel->topic());
//  m_chatView->evaluateJavaScript(msg.js());
}


void ChannelTab::sendTopic(const QString &text)
{
  Q_UNUSED(text)
//  m_bar->setVisible(false);
//  if (!text.isEmpty())
//    m_chatView->evaluateJavaScript("showTopic", true);
//
//  if (m_channel->topic().topic == text)
//    return;
//
//  ChatCore::i()->startNotify(ChatCore::SetSendFocusNotice);
//  MessageData msg(UserUtils::userId(), id(), "topic", text);
//  m_client->send(msg, true);
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


void ChannelTab::topicFocusOut()
{
//  m_bar->setVisible(false);
//  if (!m_channel->topic().topic.isEmpty())
//    m_chatView->evaluateJavaScript("showTopic", true);
//
//  ChatCore::i()->startNotify(ChatCore::SetSendFocusNotice);
}


void ChannelTab::userDataChanged(const QByteArray &userId, int changed)
{
  if (!(changed & SimpleClient::UserNickChanged))
    return;

  if (!m_channel->channels().all().contains(userId))
    return;

  ClientUser user = UserUtils::user(userId);
  if (!user)
    return;

  UserUtils::updateUserNick(m_chatView, user);
}


void ChannelTab::userLeave(const QByteArray &userId)
{
  if (m_userView->remove(userId)) {
//    addQuitMsg(userId, m_channel->id());
    displayUserCount();
  }
}


/*!
 * \deprecated Эта функция больше не используется.
 */
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
