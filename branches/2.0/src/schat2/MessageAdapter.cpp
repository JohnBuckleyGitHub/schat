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

#include <QHashIterator>

#include "debugstream.h"
#include "MessageAdapter.h"
#include "net/packets/message.h"
#include "net/SimpleClient.h"

MessageAdapter::MessageAdapter(SimpleClient *client)
  : QObject(client)
  , m_richText(false)
  , m_name(0)
  , m_client(client)
{
  connect(m_client, SIGNAL(allDelivered(quint64)), SLOT(allDelivered(quint64)));
  connect(m_client, SIGNAL(message(const MessageData &)), SLOT(clientMessage(const MessageData &)));
}


/*!
 * Отправка сообщения, если в сообщении содержаться команды то они будут обработаны.
 */
int MessageAdapter::send(MessageData &data)
{
  if (data.text.isEmpty())
    return ErrorTextEmpty;

  QString text;

  if (m_richText) {
    text = MessageUtils::toPlainText(data.text);
  }
  else {
    text = data.text.trimmed();
  }

  SCHAT_DEBUG_STREAM(this << "send()" << text)

  if (text.at(0) != '/') {
    sendText(data);
    return SentAsText;
  }

  if (text.startsWith("/join ", Qt::CaseInsensitive)) {
    data.command = "join";
    data.text = text.mid(6);
    sendCommand(data);
  }
  else
    return NoSent;

  return SentAsCommand;
}


/*!
 * Обработка уведомления о доставке пакетов на транспортном уровне.
 */
void MessageAdapter::allDelivered(quint64 id)
{
  Q_UNUSED(id)

  if (m_undelivered.isEmpty())
    return;

  QHashIterator<quint64, QByteArray> i(m_undelivered);
  while (i.hasNext()) {
    i.next();
    MessageData data(m_client->userId(), i.value(), "");
    data.name = i.key();
    emit message(OutgoingMessage | PartiallyConfirmed, data);
  }

  m_undelivered.clear();
}


/*!
 * Обработка получения нового сообщения от клиента.
 */
void MessageAdapter::clientMessage(const MessageData &data)
{
  if (data.senderId != m_client->userId())
    emit message(IncomingMessage, data);
}


bool MessageAdapter::sendCommand(MessageData &data)
{
  data.senderId.clear();
  data.destId.clear();
  data.autoSetOptions();

  return m_client->send(data);
}


/*!
 * Отправка сообщения.
 */
bool MessageAdapter::sendText(MessageData &data)
{
  if (data.senderId.isEmpty())
    data.senderId = m_client->userId();

  ++m_name;
  data.name = m_name;
  data.options |= MessageData::NameOption;

  if (m_client->send(data)) {
    emit message(OutgoingMessage | Undelivered, data);
    m_undelivered.insert(m_name, data.destId);
    return true;
  }

  --m_name;
  return false;
}
