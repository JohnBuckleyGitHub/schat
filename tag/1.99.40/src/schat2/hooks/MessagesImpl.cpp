/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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
#include "client/ClientMessages.h"
#include "client/SimpleClient.h"
#include "hooks/MessagesImpl.h"
#include "messages/ChannelMessage.h"
#include "net/packets/MessageNotice.h"
#include "sglobal.h"
#include "text/LinksFilter.h"
#include "text/UrlFilter.h"
#include "ui/TabWidget.h"

namespace Hooks
{

MessagesImpl::MessagesImpl(QObject *parent)
  : Messages(parent)
{
  ChatClient::messages()->hooks()->add(this);

  TokenFilter::add(LS("channel"), new LinksFilter());
  TokenFilter::add(LS("channel"), new UrlFilter());

  connect(ChatClient::io(), SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int, int)));
}


/*!
 * Чтение полученного сообщения.
 */
int MessagesImpl::readText(MessagePacket packet)
{
  QString command = packet->command();
  if (command != LS("m") && command != LS("me") && command != LS("say"))
    return 0;

  ChannelMessage message(packet);
  TabWidget::add(message);

  if (packet->status() == Notice::Found || packet->status() == Notice::Read)
    return 1;

  if (packet->sender() != ChatClient::id() || !m_undelivered.contains(packet->id())) {
    MessageAlert alert(message);
    ChatAlerts::start(alert);
  }

  m_undelivered.remove(packet->internalId());
  return 1;
}


/*!
 * Обработка отправки сообщения, пакет сообщения добавляется в список не доставленных сообщений
 * и происходи немедленное отображение сообщения в пользовательском интерфейсе со статусом "undelivered".
 */
void MessagesImpl::sendText(MessagePacket packet)
{
  ChannelMessage message(packet);
  message.data()[LS("Status")] = LS("undelivered");

  m_undelivered[packet->id()] = packet;
  TabWidget::add(message);
}


void MessagesImpl::unhandled(MessagePacket packet) const
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

  /// В случае если предыдущее состояние клиента было в "В сети" и имеются не доставленные
  /// сообщения, то они помечаются недоставленными для отображения в пользовательском интерфейсе.
  if (previousState == SimpleClient::ClientOnline && !m_undelivered.isEmpty()) {
    QHashIterator<QByteArray, MessagePacket> i(m_undelivered);
    while (i.hasNext()) {
      i.next();
      ChannelMessage message(i.value());
      message.data()[LS("Status")] = LS("rejected");

      TabWidget::add(message);
    }

    m_undelivered.clear();
  }
}

} // namespace Hooks