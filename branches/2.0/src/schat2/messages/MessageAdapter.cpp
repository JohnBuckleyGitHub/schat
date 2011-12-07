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

#include "actions/ChatViewAction.h"
#include "ChatCore.h"
#include "ChatPlugins.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientCmd.h"
#include "client/SimpleClient.h"
#include "debugstream.h"
#include "messages/AlertMessage.h"
#include "messages/MessageAdapter.h"
#include "messages/MessageBox.h"
#include "messages/TopicMessage.h"
#include "messages/UserMessage.h"
#include "net/packets/ChannelPacket.h"
#include "net/packets/Notice.h"
#include "net/packets/users.h"
#include "NetworkManager.h"
#include "plugins/hooks.h"
#include "text/LinksFilter.h"
#include "text/TokenFilter.h"
#include "text/UrlFilter.h"
#include "ui/ChannelUtils.h"
#include "ui/UserUtils.h"
#include "User.h"

MessageAdapter::MessageAdapter()
  : ClientHelper(ChatCore::i()->client())
  , m_settings(ChatCore::i()->settings())
{
//  TokenFilter::add(QLatin1String("user-type"), new LinksFilter());
//  TokenFilter::add(QLatin1String("user-type"), new UrlFilter());

  m_richText = true;
  connect(m_client, SIGNAL(message(const MessageData &)), SLOT(message(const MessageData &)));
  connect(m_client, SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int, int)));
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
 * - /help
 * - /hide
 * - /ignore
 * - /join
 * - /login
 * - /male
 * - /me
 * - /nick
 * - /offline
 * - /online
 * - /open
 * - /quit
 * - /reg
 * - /set
 * - /topic
 * - /unignore
 */
void MessageAdapter::command(const ClientCmd &cmd)
{
  QString command = cmd.command().toLower();

  if (command == QLatin1String("about")) {
    ChatCore::i()->openUrl(QLatin1String("chat://about"));
    return;
  }

  if (command == QLatin1String("away")) {
    setStatus(Status::Away, cmd.body());
    return;
  }

  if (command == QLatin1String("color")) {
    if (cmd.isBody())
      setGender(QString(), cmd.body());
    else
      setGender(QString(), QLatin1String("default"));

    return;
  }

  if (command == QLatin1String("dnd")) {
    setStatus(Status::DnD, cmd.body());
    return;
  }

  if (command == QLatin1String("exit") || command == QLatin1String("quit")) {
    ChatCore::i()->startNotify(ChatCore::QuitNotice);
    return;
  }

  if (command == QLatin1String("female")) {
    setGender(command, cmd.body());
    return;
  }

  if (command == QLatin1String("ffc")) {
    setStatus(Status::FreeForChat, cmd.body());
    return;
  }

  if (command == QLatin1String("gender") && cmd.isBody()) {
    setGender(cmd.body(), QString());
    return;
  }

  if (command == QLatin1String("help")) {
    commandHelpHint(QString());
    return;
  }

  if (command == QLatin1String("hide")) {
    ChatCore::i()->startNotify(ChatCore::ToggleVisibilityNotice);
    return;
  }

  if (command == QLatin1String("ignore") || command == QLatin1String("unignore")) {
    if (SimpleID::typeOf(m_destId) == SimpleID::UserId)
      ChatCore::i()->openUrl(UserUtils::toUrl(UserUtils::user(m_destId), command));
    return;
  }

  if (command == "join") {
    ChatClient::channels()->join(cmd.body());
    return;
  }

  if (command == "login" || command == "reg") {
    ClientCmd body(cmd.body());
    if (body.isValid() && body.isBody())
      login(command, body.command(), body.body());

    return;
  }

  if (command == QLatin1String("male")) {
    setGender(command, cmd.body());
    return;
  }

  if (command == QLatin1String("nick") && cmd.isBody() && cmd.body().size() >= 3) {
    m_settings->updateValue(QLatin1String("Profile/Nick"), cmd.body());
    return;
  }

  if (command == QLatin1String("offline")) {
    setStatus(Status::Offline, cmd.body());
    return;
  }

  if (command == QLatin1String("online")) {
    setStatus(Status::Online, cmd.body());
    return;
  }

  if (command == QLatin1String("open") && cmd.isBody()) {
    ChatCore::i()->openUrl(cmd.body());
    return;
  }

  if (command == QLatin1String("set")) {
    ClientCmd body(cmd.body());
    if (body.isValid() && body.isBody())
      m_settings->setValue(body.command(), body.body());

    return;
  }

  if (command == "topic") {
    if (SimpleID::typeOf(m_destId) == SimpleID::ChannelId) {
      MessageData msg(UserUtils::userId(), m_destId, command, QString());
      m_client->send(msg, true);
    }

    return;
  }

  commandHelpHint(command);
}


void MessageAdapter::notice()
{
  QString command = m_notice->command();

  if (command == "msg.rejected") {
    newUserMessage(UserMessage::OutgoingMessage | UserMessage::Rejected, msgFromNotice());
  }
  else if (command == "msg.accepted" && m_notice->status() == Notice::UserOffline) {
    newUserMessage(UserMessage::OutgoingMessage | UserMessage::Delivered | UserMessage::Offline, msgFromNotice());
  }
}


/*!
 * Обработка получения нового сообщения от клиента.
 */
void MessageAdapter::message(const MessageData &data)
{
  if (data.command == QLatin1String("topic")) {
    readTopic(data);
    return;
  }

  if (ChatCore::i()->isIgnored(data.senderId)) {
    if (data.destId() != m_client->channelId())
      return;

    Notice notice(UserUtils::userId(), data.senderId, "msg.rejected", data.timestamp, data.id);
    notice.setStatus(Notice::Forbidden);
    m_client->send(notice.data(m_client->sendStream()));
    return;
  }

  int status = UserMessage::IncomingMessage;
  if (data.senderId == UserUtils::userId()) {
    status = UserMessage::OutgoingMessage;
    if (m_undelivered.contains(data.id))
      status |= UserMessage::Delivered;
  }

  newUserMessage(status, data);
}


/*!
 * \todo Улучшить функциональность уведомлений об подключении и отключении.
 */
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

  setStateAll(UserMessage::Rejected);
}


int MessageAdapter::setGender(const QString &gender, const QString &color)
{
  if (gender.isEmpty() && color.isEmpty())
    return CommandArgsError;

//  User user(m_client->user().data());
//
//  if (!gender.isEmpty()) {
//    if (gender == QLatin1String("male"))
//      user.gender().set(Gender::Male);
//    else if (gender == QLatin1String("female"))
//      user.gender().set(Gender::Female);
//    else if (gender == QLatin1String("ghost"))
//      user.gender().set(Gender::Ghost);
//    else if (gender == QLatin1String("unknown"))
//      user.gender().set(Gender::Unknown);
//    else if (gender == QLatin1String("bot"))
//      user.gender().set(Gender::Bot);
//    else
//      return CommandArgsError;
//  }
//
//  if (!color.isEmpty()) {
//    user.gender().setColor(UserUtils::color(color));
//  }
//
//  if (m_client->user()->gender().raw() == user.gender().raw())
//    return NoSent;
//
//  m_settings->updateValue(QLatin1String("Profile/Gender"), user.gender().raw());
  return SentAsCommand;
}


MessageData MessageAdapter::msgFromNotice() const
{
  MessageData msg(m_client->channelId(), m_notice->sender(), QString(), QString());
  msg.id = m_notice->id();
  msg.timestamp = m_notice->date();
  return msg;
}


void MessageAdapter::commandHelpHint(const QString &command)
{
  QString text;
  if (!command.isEmpty())
    text += tr("Unknown command <b>/%1</b>.<br />").arg(command);

  text += tr("Would you like to see help for commands on site <a href='http://simple.impomezia.com/Commands'>http://simple.impomezia.com</a>?");
  MessageBox box(QLatin1String("yes-no-box"), text, m_destId);
  UrlAction *action = new UrlAction(QUrl(QLatin1String("http://simple.impomezia.com/Commands")));
  ChatCore::i()->addChatViewAction(box.id(), action);

  emit message(box);
}


/*!
 * Формирование нового сообщения от пользователя.
 */
void MessageAdapter::newUserMessage(int status, const MessageData &data)
{
  int priority = UserMessage::NormalPriority;

  if (status & UserMessage::OutgoingMessage && !(status & UserMessage::Undelivered)) {
    priority = UserMessage::IdlePriority;

    if (m_undelivered.contains(data.id)) {
      UserMessage msg(status, data);
      msg.setPriority(priority);

      emit message(msg);

      RawUserMessageHook hook(status, data);
      ChatCore::i()->plugins()->hook(hook);
      m_undelivered.remove(data.id);
      return;
    }
  }

  UserMessage msg(status, data);
  msg.setPriority(priority);
  emit message(msg);

  if (status & UserMessage::OutgoingMessage && status & UserMessage::Undelivered) {
    m_undelivered[data.id] = data;
  }
  else {
    RawUserMessageHook hook(status, data);
    ChatCore::i()->plugins()->hook(hook);
  }
}


void MessageAdapter::readTopic(const MessageData &data)
{
  Q_UNUSED(data)
//  ClientChannel channel = m_client->channel(data.destId());
//  if (!channel)
//    return;
//
//  if (channel->topic().topic == data.text)
//    return;
//
//  channel->setTopic(data.text, data.senderId, data.timestamp);
//  TopicMessage msg(channel->topic());
//  emit message(msg);
//  emit channelDataChanged(data.senderId, channel->id());
}


/*!
 * Устанавливает состояние для всех пакетов с неподтверждённой доставкой.
 */
void MessageAdapter::setStateAll(int state)
{
  if (m_undelivered.isEmpty())
    return;

  QHashIterator<QByteArray, MessageData> i(m_undelivered);
  while (i.hasNext()) {
    i.next();
    newUserMessage(UserMessage::OutgoingMessage | state, i.value());
  }
}


void MessageAdapter::setStatus(int status, const QString &text)
{
  if (text.isEmpty())
    m_settings->updateValue(QLatin1String("Profile/Status"), status);
  else
    m_settings->updateValue(QLatin1String("Profile/Status"), User::statusToString(status, text));
}
