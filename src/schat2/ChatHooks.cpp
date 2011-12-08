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

#include "ChatCore.h"
#include "ChatHooks.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/ClientMessages.h"
#include "client/SimpleClient.h"
#include "messages/ChannelMessage.h"
#include "net/packets/MessagePacket.h"
#include "net/SimpleID.h"
#include "NetworkManager.h"
#include "text/LinksFilter.h"
#include "text/TokenFilter.h"
#include "text/UrlFilter.h"
#include "ui/TabWidget.h"

namespace Hooks {

ChatMessages::ChatMessages(QObject *parent)
  : Messages(parent)
{
  ChatClient::messages()->hooks()->add(this);

  TokenFilter::add("channel", new LinksFilter());
  TokenFilter::add("channel", new UrlFilter());

  connect(ChatClient::io(), SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int, int)));
}


/*!
 * Чтение полученного сообщения.
 */
void ChatMessages::readText(const MessagePacket &packet)
{
  m_undelivered.remove(packet.id());

  ChannelMessage message(packet);

  /// Если это собственное сообщение, то для него при необходимости устанавливается
  /// статус "offline" или "rejected".
  if (packet.sender() == ChatClient::id()) {
    if (packet.status() == Notice::UserOffline)
      message.data()["Status"] = "offline";
    else if (packet.status() != Notice::OK)
      message.data()["Status"] = "rejected";
  }

  TabWidget::i()->add(message);
}


/*!
 * Обработка отправки сообщения, пакет сообщения добавляется в список не доставленных сообщений
 * и происходи немедленное отображение сообщения в пользовательском интерфейсе со статусом "undelivered".
 */
void ChatMessages::sendText(const MessagePacket &packet)
{
  ChannelMessage message(packet);
  message.data()["Status"] = "undelivered";

  m_undelivered[packet.id()] = packet;
  TabWidget::i()->add(message);
}


/*!
 * Обработка изменения состояния клиента.
 */
void ChatMessages::clientStateChanged(int state, int previousState)
{
  Q_UNUSED(state)

  /// В случае если предыдущее состояние клиента было в "В сети" и имеются не доставленные
  /// сообщения, то они помечаются недоставленными для отображения в пользовательском интерфейсе.
  if (previousState == SimpleClient::ClientOnline && !m_undelivered.isEmpty()) {
    QHashIterator<QByteArray, MessagePacket> i(m_undelivered);
    while (i.hasNext()) {
      i.next();
      ChannelMessage message(i.value());
      message.data()["Status"] = "rejected";

      TabWidget::i()->add(message);
    }

    m_undelivered.clear();
  }
}


Networks::Networks(QObject *parent)
  : Client(parent)
{
  ChatClient::i()->hooks()->add(this);
}


bool Networks::openId(const QByteArray &id, bool *matched)
{
  *matched = true;

  if (id.isEmpty())
    return ChatCore::networks()->open();

  return ChatCore::networks()->open(id);
}


/*!
 * Получение идентификатора пользователя из менеджера сетей
 * и установка базовых данных из настроек.
 */
QByteArray Networks::id()
{
  ClientChannel channel = ChatClient::channel();
  channel->setName(ChatCore::settings()->value("Profile/Nick").toString());
  channel->gender().setRaw(ChatCore::settings()->value("Profile/Gender").toInt());
  channel->status().set(ChatCore::settings()->value("Profile/Status").toInt());

  Network item = ChatCore::networks()->item(ChatCore::networks()->selected());
  if (!item->isValid())
    return QByteArray();

  channel->setId(item->userId());

  return channel->id();
}


/*!
 * Получение идентификатора сервера из менеджера сетей.
 */
QByteArray Networks::serverId()
{
  return ChatCore::networks()->selected();
}

}  // namespace Hooks
