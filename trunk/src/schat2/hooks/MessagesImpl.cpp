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

#include "alerts/MessageAlert.h"
#include "ChatAlerts.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientMessages.h"
#include "client/SimpleClient.h"
#include "feeds/FeedStrings.h"
#include "hooks/MessagesImpl.h"
#include "messages/ChannelMessage.h"
#include "net/packets/MessageNotice.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "text/LinksFilter.h"
#include "text/UrlFilter.h"
#include "ui/TabWidget.h"

namespace Hooks
{

MessagesImpl::MessagesImpl(QObject *parent)
  : Messages(parent)
{
  ChatClient::messages()->add(this);

  TokenFilter::add(LS("channel"), new LinksFilter());
  TokenFilter::add(LS("channel"), new UrlFilter());

  connect(ChatClient::io(), SIGNAL(clientStateChanged(int,int)), SLOT(clientStateChanged(int,int)));
  connect(ChatClient::i(), SIGNAL(ready()), SLOT(ready()));
}


/*!
 * Чтение полученного сообщения.
 */
int MessagesImpl::read(MessagePacket packet)
{
  const QString &command = packet->command();
  if (command != LS("m") && command != LS("me") && command != LS("say"))
    return 0;

  if (SimpleID::typeOf(packet->dest()) == SimpleID::ChannelId && ignored(ChatClient::channels()->get(packet->sender())))
    return 0;

  ChannelMessage message(packet);
  if (referring(message))
    message.data()[LS("Status")] = LS("referring");

  TabWidget::add(message, !(packet->sender() == ChatClient::id() && m_undelivered.contains(packet->internalId())));

  if (packet->status() == Notice::Found || packet->status() == Notice::Read)
    return 1;

  if (packet->sender() != ChatClient::id() || !m_undelivered.contains(packet->internalId())) {
    MessageAlert alert(message);
    ChatAlerts::start(alert);
  }

  m_undelivered.remove(packet->internalId());
  return 1;
}


/*!
 * \return \b true если сообщения пользователя игнорируются.
 */
bool MessagesImpl::ignored(ClientChannel user)
{
  if (!user || user->type() != SimpleID::UserId)
    return false;

  FeedPtr feed = ChatClient::channel()->feed(FEED_NAME_ACL, false);
  if (!feed)
    return false;

  return !feed->can(user.data(), Acl::Write);
}


void MessagesImpl::error(MessagePacket packet)
{
  sent(packet);
}


/*!
 * Обработка отправки сообщения, пакет сообщения добавляется в список не доставленных сообщений
 * и происходи немедленное отображение сообщения в пользовательском интерфейсе со статусом "undelivered".
 */
void MessagesImpl::sent(MessagePacket packet)
{
  ChannelMessage message(packet);
  message.data()[LS("Status")] = LS("undelivered");

  m_undelivered[packet->id()] = packet;
  TabWidget::add(message);
}


void MessagesImpl::unhandled(MessagePacket packet)
{
  if (packet->direction() == Notice::Internal)
    return;

  Message message(packet->id(), Message::detectTab(packet->sender(), packet->dest()), LS("unhandled"), LS("addUnhandledMessage"));
  message.setAuthor(packet->sender());
  message.setDate(packet->date());
  message.data()[LS("Command")] = packet->command();
  message.data()[LS("Text")]    = packet->text();
  message.data()[LS("Status")]  = packet->status();
  message.data()[LS("JSON")]    = packet->json();
  TabWidget::add(message);
}


/*!
 * Обработка изменения состояния клиента.
 */
void MessagesImpl::clientStateChanged(int state, int previousState)
{
  Q_UNUSED(state)

  if (previousState == SimpleClient::ClientOnline)
    m_serverId = ChatClient::serverId();
}


void MessagesImpl::ready()
{
  if (m_serverId.isEmpty()) {
    m_serverId = ChatClient::serverId();
  }
  else if (m_serverId != ChatClient::serverId()) {
    rejectAll();
    return;
  }

  SimpleClient *client = ChatClient::io();
  QMap<qint64, MessagePacket> sorted;

  foreach (MessagePacket packet, m_undelivered)
    sorted.insert(packet->date(), packet);

  if (sorted.isEmpty())
    return;

  ChatClientLocker locker(client);
  foreach (MessagePacket packet, sorted)
    client->send(packet, true);
}


/*!
 * Определение наличия обращения к пользователю в сообщении.
 *
 * \return \b true если в тексте сообщения была обнаружена ссылка с ником пользователя.
 */
bool MessagesImpl::referring(const ChannelMessage &message) const
{
  if (SimpleID::typeOf(message.tab()) != SimpleID::ChannelId)
    return false;

  if (!message.data().value(LS("Text")).toString().contains(LS("chat://channel/") + SimpleID::encode(ChatClient::id())))
    return false;

  return true;
}


void MessagesImpl::rejectAll()
{
  foreach (MessagePacket packet, m_undelivered) {
    ChannelMessage message(packet);
    message.data()[LS("Status")] = LS("rejected");

    TabWidget::add(message);
  }

  m_undelivered.clear();
}

} // namespace Hooks
