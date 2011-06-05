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

#include "ChatCore.h"
#include "ChatMessage.h"
#include "ChatSettings.h"
#include "debugstream.h"
#include "MessageAdapter.h"
#include "net/packets/message.h"
#include "net/packets/notices.h"
#include "net/packets/users.h"
#include "net/SimpleClient.h"
#include "ui/UserUtils.h"
#include "User.h"

MessageAdapter::MessageAdapter(SimpleClient *client)
  : QObject(client)
  , m_richText(true)
  , m_name(1)
  , m_client(client)
{
  connect(m_client, SIGNAL(allDelivered(quint64)), SLOT(allDelivered(quint64)));
  connect(m_client, SIGNAL(message(const MessageData &)), SLOT(clientMessage(const MessageData &)));
  connect(m_client, SIGNAL(clientStateChanged(int)), SLOT(clientStateChanged(int)));
  connect(m_client, SIGNAL(notice(const NoticeData &)), SLOT(notice(const NoticeData &)));
}


/*!
 * Отправка сообщения, если в сообщении содержаться команды, то они будут обработаны.
 * Команды:
 * - /join &lt;имя канала&gt;
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
    text = data.text.simplified();
  }

  SCHAT_DEBUG_STREAM(this << "send()" << text)

  if (text.at(0) != '/' || text.startsWith("/me", Qt::CaseInsensitive)) {
    sendText(data);
    return SentAsText;
  }

  QStringList commands = (" " + text).split(" /", QString::SkipEmptyParts);
  for (int i = 0; i < commands.size(); ++i) {
    command(commands.at(i));
  }

  return SentAsCommand;
}


/*!
 * Обработка уведомления о доставке пакетов на транспортном уровне.
 */
void MessageAdapter::allDelivered(quint64 id)
{
  Q_UNUSED(id)
//  setStateAll(ChatMessage::Delivered, "");
}


/*!
 * Обработка получения нового сообщения от клиента.
 */
void MessageAdapter::clientMessage(const MessageData &data)
{
  if (data.senderId != m_client->userId())
    emit message(ChatMessage::IncomingMessage, data);
}


void MessageAdapter::clientStateChanged(int state)
{
  if (state == SimpleClient::ClientOnline)
    return;

  setStateAll(ChatMessage::Rejected, tr("Потерянно соединение с сервером"));
}


void MessageAdapter::notice(const NoticeData &data)
{
  SCHAT_DEBUG_STREAM(this << "notice()")

  if (m_client->userId() != data.senderId)
    return;

  if (data.type == NoticeData::MessageDelivered || data.type == NoticeData::MessageRejected) {
    MessageData msg(m_client->userId(), data.destId, "");
    msg.name = data.messageName;
    msg.timestamp = data.timestamp;

    m_undelivered.remove(data.messageName);

    if (data.type == NoticeData::MessageDelivered) {
      emit message(ChatMessage::OutgoingMessage | ChatMessage::Delivered, msg);
    }
    else {
      emit message(ChatMessage::OutgoingMessage | ChatMessage::Rejected, msg);
    }
  }
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
    emit message(ChatMessage::OutgoingMessage | ChatMessage::Undelivered, data);
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
    else if (gender == "ghost")
      user.setGender(User::Ghost);
    else if (gender == "unknown")
      user.setGender(User::Unknown);
    else
      return CommandArgsError;
  }

  if (!color.isEmpty()) {
    user.setColor(UserUtils::color(color));
  }

  if (m_client->user()->rawGender() == user.rawGender())
    return NoSent;

  ChatCore::i()->settings()->updateValue(ChatSettings::ProfileGender, user.rawGender());
  return SentAsCommand;
}


/*!
 * Обработка комманд.
 */
void MessageAdapter::command(const QString &text)
{
  if (text.startsWith("join ", Qt::CaseInsensitive) && text.size() > 7) {
    MessageData data(QByteArray(), QByteArray(), "join", text.mid(5));
    m_client->send(data);

    return;
  }

  if (text.startsWith("nick ", Qt::CaseInsensitive) && text.size() > 7) {
    ChatCore::i()->settings()->updateValue(ChatSettings::ProfileNick, text.mid(5));
    return;
  }

  if (text.startsWith("gender ", Qt::CaseInsensitive)) {
    setGender(text.mid(7).toLower(), "");
    return;
  }

  if (text.startsWith("color ", Qt::CaseInsensitive)) {
    setGender("", text.mid(6).toLower());
    return;
  }

  if (text.startsWith("color", Qt::CaseInsensitive)) {
    setGender("", "default");
    return;
  }

  if (text.startsWith("male ", Qt::CaseInsensitive)) {
    setGender("male", text.mid(5).toLower());
    return;
  }

  if (text.startsWith("female ", Qt::CaseInsensitive)) {
    setGender("female", text.mid(7).toLower());
    return;
  }

  if (text.startsWith("male", Qt::CaseInsensitive)) {
    setGender("male", "");
    return;
  }

  if (text.startsWith("female", Qt::CaseInsensitive)) {
    setGender("female", "");
    return;
  }

  if (text.startsWith("set ", Qt::CaseInsensitive)) {
    int offset = 4;
    QString key = text.mid(offset, text.indexOf(' ', offset) - offset);
    QString value = text.mid(offset + key.size() + 1);

    ChatCore::i()->settings()->setValue(key, value);
    return;
  }

  if (text.startsWith("about", Qt::CaseInsensitive)) {
    ChatCore::i()->startNotify(ChatCore::AboutNotice);
    return;
  }

  if (text.startsWith("exit", Qt::CaseInsensitive) || text.startsWith("quit", Qt::CaseInsensitive)) {
    ChatCore::i()->startNotify(ChatCore::QuitNotice);
    return;
  }

  if (text.startsWith("hide", Qt::CaseInsensitive)) {
    ChatCore::i()->startNotify(ChatCore::ToggleVisibilityNotice);
    return;
  }
}


/*!
 * Устанавливает состояние для всех пакетов с неподтверждённой доставкой.
 */
void MessageAdapter::setStateAll(int state, const QString &reason)
{
  Q_UNUSED(reason)

  if (m_undelivered.isEmpty())
    return;

  MessageData data(m_client->userId(), QByteArray(), reason);

  QHashIterator<quint64, QByteArray> i(m_undelivered);
  while (i.hasNext()) {
    i.next();
    data.name = i.key();
    data.destId = i.value();
    emit message(ChatMessage::OutgoingMessage | state, data);
  }

  m_undelivered.clear();
}
