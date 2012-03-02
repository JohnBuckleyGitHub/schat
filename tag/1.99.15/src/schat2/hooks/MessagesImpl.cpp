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
void MessagesImpl::readText(MessagePacket packet)
{
  ChannelMessage message(packet);

  if (TabWidget::i())
    TabWidget::i()->add(message);

  if (packet->status() == Notice::Found || packet->status() == Notice::Read)
    return;

  if (packet->sender() != ChatClient::id() || !m_undelivered.contains(packet->id())) {
    qDebug() << " ~~ START ALERT ~~";
    MessageAlert alert(message);
    ChatAlerts::start(alert);
  }

  m_undelivered.remove(packet->id());
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

  if (TabWidget::i())
    TabWidget::i()->add(message);
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

      if (TabWidget::i())
        TabWidget::i()->add(message);
    }

    m_undelivered.clear();
  }
}

} // namespace Hooks
