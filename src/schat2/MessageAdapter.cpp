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
#include "net/packets/users.h"
#include "net/SimpleClient.h"
#include "User.h"

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
 * Отправка сообщения, если в сообщении содержаться команды, то они будут обработаны.
 * Команды:
 * - /join <имя канала>
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

  if (text.at(0) != '/' || text.startsWith("/me", Qt::CaseInsensitive)) {
    sendText(data);
    return SentAsText;
  }

  if (text.startsWith("/join ", Qt::CaseInsensitive)) {
    data.senderId.clear();
    data.destId.clear();
    data.command = "join";
    data.text = text.mid(6);
    sendCommand(data);
    return SentAsCommand;
  }

  if (text.startsWith("/nick ", Qt::CaseInsensitive)) {
    QString nick = text.mid(6);

    if (m_client->user()->nick() != nick && User::isValidNick(nick)) {
      User user(m_client->user());
      user.setNick(nick);
      UserWriter writer(m_client->sendStream(), &user);
      m_client->send(writer.data());
      return SentAsCommand;
    }
    return CommandArgsError;
  }

  if (text.startsWith("/gender ", Qt::CaseInsensitive)) {
    return setGender(text.mid(8).toLower(), "");
  }

  if (text.startsWith("/color ", Qt::CaseInsensitive)) {
    return setGender("", text.mid(7).toLower());
  }

  if (text.startsWith("/male ", Qt::CaseInsensitive)) {
    return setGender("male", text.mid(6).toLower());
  }

  if (text.startsWith("/female ", Qt::CaseInsensitive)) {
    return setGender("female", text.mid(8).toLower());
  }

  if (text.startsWith("/male", Qt::CaseInsensitive)) {
    return setGender("male", "");
  }

  if (text.startsWith("/female", Qt::CaseInsensitive)) {
    return setGender("female", "");
  }

  return NoSent;
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


int MessageAdapter::setGender(const QString &gender, const QString &color)
{
  if (gender.isEmpty() && color.isEmpty())
    return CommandArgsError;

  User user(m_client->user());

  if (!gender.isEmpty()) {
    if (gender == "male")
      user.setGender(User::Male);
    else if (gender == "female")
      user.setGender(User::Female);
    else if (gender == "unknown")
      user.setGender(User::Unknown);
    else
      return CommandArgsError;
  }

  if (!color.isEmpty()) {
    if (color == "default")
      user.setColor(User::Default);
    else if (color == "black")
      user.setColor(User::Black);
    else if (color == "gray")
      user.setColor(User::Gray);
    else if (color == "green")
      user.setColor(User::Green);
    else if (color == "red")
      user.setColor(User::Red);
    else if (color == "white")
      user.setColor(User::White);
    else if (color == "yellow")
      user.setColor(User::Yellow);
    else if (color == "medical")
      user.setColor(User::Medical);
    else if (color == "nude")
      user.setColor(User::Nude);
    else if (color == "thief")
      user.setColor(User::Thief);
    else
      return CommandArgsError;
  }

  if (m_client->user()->rawGender() == user.rawGender())
    return NoSent;

  UserWriter writer(m_client->sendStream(), &user);
  m_client->send(writer.data());
  return SentAsCommand;
}
