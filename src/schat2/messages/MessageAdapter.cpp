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
#include "ChatSettings.h"
#include "client/SimpleClient.h"
#include "debugstream.h"
#include "messages/AlertMessage.h"
#include "messages/MessageAdapter.h"
#include "messages/UserMessage.h"
#include "net/packets/message.h"
#include "net/packets/notices.h"
#include "net/packets/users.h"
#include "NetworkManager.h"
#include "ui/UserUtils.h"
#include "User.h"

MessageAdapter::MessageAdapter()
  : ClientHelper(ChatCore::i()->client())
  , m_settings(ChatCore::i()->settings())
{
  m_richText = true;
  connect(m_client, SIGNAL(allDelivered(quint64)), SLOT(allDelivered(quint64)));
  connect(m_client, SIGNAL(message(const MessageData &)), SLOT(clientMessage(const MessageData &)));
  connect(m_client, SIGNAL(clientStateChanged(int)), SLOT(clientStateChanged(int)));
  connect(m_client, SIGNAL(notice(const NoticeData &)), SLOT(notice(const NoticeData &)));
}


bool MessageAdapter::sendText(MessageData &data)
{
  if (ClientHelper::sendText(data)) {
    newUserMessage(UserMessage::OutgoingMessage | UserMessage::Undelivered, data);
    m_undelivered.insert(m_name, data.destId);
    return true;
  }

  return false;
}


/*!
 * Обработка комманд http://simple.impomezia.com/Commands
 * - /about
 * - /away
 * - /color
 * - /dnd
 * - /exit
 * - /female
 * - /ffc
 * - /gender
 * - /hide
 * - /join
 * - /male
 * - /nick
 * - /offline
 * - /online
 * - /quit
 * - /set
 */
void MessageAdapter::command(const QString &text)
{
  if (text.startsWith("about", Qt::CaseInsensitive)) {
    ChatCore::i()->startNotify(ChatCore::AboutNotice);
    return;
  }

  if (text.startsWith("away ", Qt::CaseInsensitive)) {
    setStatus(User::AwayStatus, text.mid(5));
    return;
  }

  if (text.startsWith("away", Qt::CaseInsensitive)) {
    setStatus(User::AwayStatus);
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

  if (text.startsWith("dnd ", Qt::CaseInsensitive)) {
    setStatus(User::DnDStatus, text.mid(4));
    return;
  }

  if (text.startsWith("dnd", Qt::CaseInsensitive)) {
    setStatus(User::DnDStatus);
    return;
  }

  if (text.startsWith("exit", Qt::CaseInsensitive) || text.startsWith("quit", Qt::CaseInsensitive)) {
    ChatCore::i()->startNotify(ChatCore::QuitNotice);
    return;
  }

  if (text.startsWith("female ", Qt::CaseInsensitive)) {
    setGender("female", text.mid(7).toLower());
    return;
  }

  if (text.startsWith("female", Qt::CaseInsensitive)) {
    setGender("female", "");
    return;
  }

  if (text.startsWith("ffc ", Qt::CaseInsensitive)) {
    setStatus(User::FreeForChatStatus, text.mid(4));
    return;
  }

  if (text.startsWith("ffc", Qt::CaseInsensitive)) {
    setStatus(User::FreeForChatStatus);
    return;
  }

  if (text.startsWith("gender ", Qt::CaseInsensitive)) {
    setGender(text.mid(7).toLower(), "");
    return;
  }

  if (text.startsWith("hide", Qt::CaseInsensitive)) {
    ChatCore::i()->startNotify(ChatCore::ToggleVisibilityNotice);
    return;
  }

  if (text.startsWith("join ", Qt::CaseInsensitive) && text.size() > 7) {
    MessageData data(QByteArray(), QByteArray(), "join", text.mid(5));
    m_client->send(data);

    return;
  }

  if (text.startsWith("male ", Qt::CaseInsensitive)) {
    setGender("male", text.mid(5).toLower());
    return;
  }

  if (text.startsWith("male", Qt::CaseInsensitive)) {
    setGender("male", "");
    return;
  }

  if (text.startsWith("nick ", Qt::CaseInsensitive) && text.size() > 7) {
    m_settings->updateValue(ChatSettings::ProfileNick, text.mid(5));
    return;
  }

  if (text.startsWith("offline ", Qt::CaseInsensitive)) {
    setStatus(User::OfflineStatus, text.mid(8));
    return;
  }

  if (text.startsWith("offline", Qt::CaseInsensitive)) {
    setStatus(User::OfflineStatus);
    return;
  }

  if (text.startsWith("online ", Qt::CaseInsensitive)) {
    setStatus(User::OnlineStatus, text.mid(7));
    return;
  }

  if (text.startsWith("online", Qt::CaseInsensitive)) {
    setStatus(User::OnlineStatus);
    return;
  }

  if (text.startsWith("set ", Qt::CaseInsensitive)) {
    int offset = 4;
    QString key = text.mid(offset, text.indexOf(' ', offset) - offset);
    QString value = text.mid(offset + key.size() + 1);

    m_settings->setValue(key, value);
    return;
  }
}


/*!
 * Обработка уведомления о доставке пакетов на транспортном уровне.
 */
void MessageAdapter::allDelivered(quint64 id)
{
  Q_UNUSED(id)
}


/*!
 * Обработка получения нового сообщения от клиента.
 */
void MessageAdapter::clientMessage(const MessageData &data)
{
  if (data.senderId != m_client->userId())
    newUserMessage(UserMessage::IncomingMessage, data);
}


void MessageAdapter::clientStateChanged(int state)
{
  if (state == SimpleClient::ClientOnline) {
    AlertMessage msg(AlertMessage::Information, tr("Successfully connected to <b>%1</b>").arg(NetworkManager::currentServerName()));
    emit message(msg);
    return;
  }
  else if (state == SimpleClient::ClientOffline) {
    AlertMessage msg(AlertMessage::Exclamation, tr("Connection lost"));
    emit message(msg);
  }

  setStateAll(UserMessage::Rejected, "rj");
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
      newUserMessage(UserMessage::OutgoingMessage | UserMessage::Delivered, msg);
    }
    else {
      newUserMessage(UserMessage::OutgoingMessage | UserMessage::Rejected, msg);
    }
  }
}


int MessageAdapter::setGender(const QString &gender, const QString &color)
{
  if (gender.isEmpty() && color.isEmpty())
    return CommandArgsError;

  User user(m_client->user().data());

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

  m_settings->updateValue(ChatSettings::ProfileGender, user.rawGender());
  return SentAsCommand;
}


/*!
 * Формирование сообщения с типом AbstractMessage::UserMessageType
 * и отправка сообщения.
 */
void MessageAdapter::newUserMessage(int status, const MessageData &data)
{
  UserMessage msg(status, data);
  emit message(msg);
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
    newUserMessage(UserMessage::OutgoingMessage | state, data);
  }

  m_undelivered.clear();
}


void MessageAdapter::setStatus(int status, const QString &text)
{
  if (text.isEmpty())
    m_settings->updateValue(ChatSettings::ProfileStatus, status);
  else
    m_settings->updateValue(ChatSettings::ProfileStatus, User::statusToString(status, text));
}
