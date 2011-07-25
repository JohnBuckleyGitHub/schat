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
#include "client/ClientCmd.h"
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
  connect(m_client, SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int, int)));
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
void MessageAdapter::command(const ClientCmd &cmd)
{
  QString command = cmd.command().toLower();

  if (command == "about") {
    ChatCore::i()->startNotify(ChatCore::AboutNotice);
    return;
  }

  if (command == "away") {
    setStatus(User::AwayStatus, cmd.body());
    return;
  }

  if (command == "color") {
    if (cmd.isBody())
      setGender("", cmd.body());
    else
      setGender("", "default");

    return;
  }

  if (command == "dnd") {
    setStatus(User::DnDStatus, cmd.body());
    return;
  }

  if (command == "exit" || command == "quit") {
    ChatCore::i()->startNotify(ChatCore::QuitNotice);
    return;
  }

  if (command == "female") {
    setGender("female", cmd.body());
    return;
  }

  if (command == "ffc") {
    setStatus(User::FreeForChatStatus, cmd.body());
    return;
  }

  if (command == "gender" && cmd.isBody()) {
    setGender(cmd.body(), "");
    return;
  }

  if (command == "hide") {
    ChatCore::i()->startNotify(ChatCore::ToggleVisibilityNotice);
    return;
  }

  if (command == "join" && cmd.isBody() && cmd.body().size() >= 3) {
    MessageData data(QByteArray(), QByteArray(), command, cmd.body());
    m_client->send(data);
    return;
  }

  if (command == "male") {
    setGender("male", cmd.body());
    return;
  }

  if (command == "nick" && cmd.isBody() && cmd.body().size() >= 3) {
    m_settings->updateValue(ChatSettings::ProfileNick, cmd.body());
    return;
  }

  if (command == "offline") {
    setStatus(User::OfflineStatus, cmd.body());
    return;
  }

  if (command == "online") {
    setStatus(User::OnlineStatus, cmd.body());
    return;
  }

  if (command == "set") {
    ClientCmd body(cmd.body());
    if (body.isValid() && body.isBody())
      m_settings->setValue(body.command(), body.body());

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


void MessageAdapter::clientStateChanged(int state, int previousState)
{
  if (state == SimpleClient::ClientOnline) {
    AlertMessage msg(AlertMessage::Information, tr("Successfully connected to <b>%1</b>").arg(NetworkManager::currentServerName()));
    emit message(msg);
    return;
  }
  else if (state == SimpleClient::ClientOffline && previousState == SimpleClient::ClientOnline) {
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
    else if (gender == "bot")
      user.setGender(User::Bot);
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
