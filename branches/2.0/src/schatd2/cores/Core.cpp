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

#include <QCoreApplication>
#include <QEvent>
#include <QThread>

#include "Account.h"
#include "cores/AnonymousAuth.h"
#include "cores/CookieAuth.h"
#include "cores/Core.h"
#include "NodeChannels.h"
#include "NodeNoticeReader.h"
#include "cores/PasswordAuth.h"
#include "DateTime.h"
#include "debugstream.h"
#include "events.h"
#include "net/PacketReader.h"
#include "net/packets/auth.h"
#include "net/packets/ChannelPacket.h"
#include "net/packets/message.h"
#include "net/packets/Notice.h"
#include "net/packets/users.h"
#include "net/PacketWriter.h"
#include "net/Protocol.h"
#include "net/ServerData.h"
#include "net/SimpleID.h"
#include "NodeLog.h"
#include "NodePlugins.h"
#include "plugins/NodeHooks.h"
#include "ServerUser.h"
#include "Storage.h"

Core::Core(QObject *parent)
  : QObject(parent)
  , m_plugins(0)
  , m_timestamp(0)
  , m_listener(0)
  , m_settings(Storage::i()->settings())
  , m_storage(Storage::i())
{
  addAuth(new AnonymousAuth(this));
  addAuth(new CookieAuth(this));

  if (Storage::i()->serverData()->is(ServerData::PasswordAuthSupport))
    addAuth(new PasswordAuth(this));

  m_sendStream = new QDataStream(&m_sendBuffer, QIODevice::ReadWrite);
  m_readStream = new QDataStream(&m_readBuffer, QIODevice::ReadWrite);

  NodeNoticeReader::add(new NodeChannels(this));
}


Core::~Core()
{
  delete m_sendStream;
  delete m_readStream;
  qDeleteAll(m_auth);
}


/*!
 * Отправка пакета всем пользователям в канале.
 *
 * \param channel Канал.
 * \param packet  Пакет.
 * \return false в случае ошибки.
 */
bool Core::send(ChatChannel channel, const QByteArray &packet)
{
  return send(m_storage->sockets(channel), QList<QByteArray>() << packet);
}


/*!
 * Отправка пакетов всем пользователям в канале.
 * \param channel Канал.
 * \param packets Пакеты.
 * \return false в случае ошибки, иначе true.
 */
bool Core::send(ChatChannel channel, const QList<QByteArray> &packets)
{
  return send(m_storage->sockets(channel), packets);
}


/*!
 * Отправка пакетов пользователю.
 */
bool Core::send(ChatUser user, const QByteArray &packet, int option)
{
  if (!user)
    return false;

  return send(user->sockets(), QList<QByteArray>() << packet, option, user->id());
}


/*!
 * Отправка пакетов пользователю.
 */
bool Core::send(ChatUser user, const QList<QByteArray> &packets, int option)
{
  if (!user)
    return false;

  return send(user->sockets(), packets, option, user->id());
}


/*!
 * Отправка пакета списку сокетов.
 */
bool Core::send(const QList<quint64> &sockets, const QByteArray &packet, int option, const QByteArray &userId)
{
  return send(sockets, QList<QByteArray>() << packet, option, userId);
}


/*!
 * Базовая функция отправки пакетов.
 * Эта функция используется всеми остальными функциями отправки пакетов.
 *
 * \param sockets Список номеров сокетов.
 * \param packets Готовые сформированные пакеты.
 * \param option  Опция сокета NewPacketsEvent::Option.
 * \param userId  Идентификатор пользователя.
 */
bool Core::send(const QList<quint64> &sockets, const QList<QByteArray> &packets, int option, const QByteArray &userId)
{
  if (sockets.isEmpty())
    return true;

  if (m_timestamp == 0)
    m_timestamp = DateTime::utc();

  NewPacketsEvent *event = new NewPacketsEvent(sockets, packets, userId);
  event->timestamp = m_timestamp;
  event->option = option;

  QCoreApplication::postEvent(m_listener, event);
  return true;
}


QByteArray Core::id() const
{
  return m_storage->serverData()->id();
}


bool Core::add(ChatChannel channel, int authType, const QByteArray &authId)
{
  Q_UNUSED(authType);
  Q_UNUSED(authId)
  return m_storage->add(channel);
}


void Core::customEvent(QEvent *event)
{
  SCHAT_DEBUG_STREAM(this << "customEvent()" << event->type())

  switch (event->type()) {
    case ServerEvent::NewPackets:
      newPacketsEvent(static_cast<NewPacketsEvent*>(event));
      break;

    case ServerEvent::SocketRelease:
      release(static_cast<SocketReleaseEvent*>(event));
      break;

    default:
      break;
  }
}


/*!
 * Маршрутизация входящих пакетов.
 */
bool Core::route()
{
  if (m_timestamp == 0)
    m_timestamp = DateTime::utc();

  return send(echoFilter(m_storage->sockets(m_reader->destinations())), m_readBuffer);
}


bool Core::checkPacket()
{
  if (m_storage->isAllowSlaves() && m_storage->isSlave(m_packetsEvent->channelId()))
    return true;

  if (m_reader->sender().isEmpty())
    return false;

  if (m_packetsEvent->channelId() != m_reader->sender())
    return false;

  return true;
}


/*!
 * Обработка новых пакетов.
 */
void Core::newPacketsEvent(NewPacketsEvent *event)
{
  m_packetsEvent = event;
  QList<QByteArray> packets = event->packets;

  SCHAT_DEBUG_STREAM(this << "newPacketsEvent()" << packets.size())

  while (!packets.isEmpty()) {
    m_readBuffer = packets.takeFirst();
    PacketReader reader(m_readStream);
    m_reader = &reader;

    if (reader.type() == Protocol::AuthRequestPacket) {
      auth();
      continue;
    }

    /// Идентификатор клиента не должен быть пустым или не верным.
    if (event->channelId().isEmpty() || m_storage->channel(event->channelId()) == 0)
      continue;

    if (!checkPacket())
      continue;

    m_timestamp = 0;
    packet(reader.type());
  }
}


void Core::packet(int type)
{
  qDebug() << "Core::readPacket()" << type;

  switch (type) {
    case Protocol::MessagePacket:
      message();
      break;

    case Protocol::UserDataPacket:
      readUserData();
      break;

    case Protocol::NoticePacket:
      notice(m_reader->get<quint16>());
      break;

    default:
      route();
      break;
  }
}


/*!
 * Формирование списка пакетов для синхронизации списка пользователей в канале.
 * Будут отправлены только данные пользователей, которые не известны получателю.
 *
 * \param channel Канал, который необходимо синхронизировать.
 * \param user    Получатель пакетов.
 *
 * \deprecated Теперь клиент самостоятельно запрашивает список пользователей.
 */
QList<QByteArray> Core::userDataToSync(ChatChannel channel, ChatUser user)
{
  QList<QByteArray> channels = user->channels();
  channels.removeAll(channel->id());

  QList<QByteArray> users; // Список пользователей данные о которых имеются у \p user.
  for (int i = 0; i < channels.size(); ++i) {
    ChatChannel channel = m_storage->channel(channels.at(i));
    if (!channel)
      continue;

    foreach (QByteArray id, channel->channels().all()) {
      if (!users.contains(id))
        users.append(id);
    }
  }

  QList<QByteArray> diff; // Список пользователей данные о которых не имеются у \p user.
  foreach (QByteArray id, channel->channels().all()) {
    if (!users.contains(id))
      diff.append(id);
  }

  if (diff.isEmpty())
    return diff;

  QList<QByteArray> packets;

  for (int i = 0; i < diff.size(); ++i) {
    ChatUser u = m_storage->user(diff.at(i));
    if (!u)
      continue;

    packets.append(UserWriter(m_sendStream, u.data(), user->id(), channel->id(), UserWriter::StaticData).data());
  }

  MessageData message(channel->id(), user->id(), QLatin1String("synced"), QString());
  packets.append(MessageWriter(m_sendStream, message).data());

  return packets;
}


/*!
 * Фильтрует список сокетов, в зависимости от необходимости эха.
 */
QList<quint64> Core::echoFilter(const QList<quint64> &sockets)
{
  ChatUser user = m_storage->user(m_reader->sender());

  if (!user)
    return sockets;

  QList<quint64> out = sockets;

  if (!m_reader->is(Protocol::EnableEcho)) {
    QList<quint64> sockets = user->sockets();
    foreach (quint64 socket, sockets) {
      out.removeAll(socket);
    }
  }
  else
    Storage::merge(out, user->sockets());

  return out;
}


/*!
 * Обработка авторизации пользователя.
 */
bool Core::auth()
{
  AuthRequest data(m_reader);

  if (!data.isValid()) {
    AuthResult result(Notice::BadRequest, data.id, NewPacketsEvent::KillSocketOption);
    reject(result);
    return false;
  }

  for (int i = 0; i < m_auth.size(); ++i) {
    if (data.authType != m_auth.at(i)->type())
      continue;

    AuthResult result = m_auth.at(i)->auth(data);
    if (result.action == AuthResult::Reject) {
      reject(result);
      return false;
    }
    else if (result.action == AuthResult::Accept) {
      accept(result);
      return true;
    }
    else if (result.action == AuthResult::Pending)
      return true;
  }

  AuthResult result(Notice::NotImplemented, data.id, NewPacketsEvent::KillSocketOption);
  reject(result);
  return false;
}


/*!
 * Успешная авторизация пользователя.
 * \todo Улучшить поддержку множественного входа, в настоящее время данные не корректно синхронизируются.
 */
void Core::accept(const AuthResult &result)
{
  ChatChannel channel = m_storage->channel(result.id);
  if (!channel)
    return;

  if (!channel->account())
    return;

  QList<QByteArray> packets;
  if (result.packet) {
    AuthReply reply(m_storage->serverData(), channel.data(), channel->account()->cookie(), result.authId, result.json);
    packets.append(reply.data(m_sendStream));
  }

  packets.append(ChannelPacket::channel(channel, channel->id(), m_sendStream));

  send(channel->sockets(), packets, result.option, channel->id());

//  ChatUser user = m_storage->user(result.id);
//  if (!user)
//    return;

//  SCHAT_LOG_INFO() << "Accept Auth" << (user->nick() + "@" + user->host() + "/" + SimpleID::encode(user->id())) << "cookie:" << SimpleID::encode(user->cookie());

//  ChatChannel channel = addChannel(user);
//  QList<QByteArray> packets;
//
//  if (channel->channels().all().size() > 1)
//    packets = userDataToSync(channel, user);
//
//  if (result.packet) {
//    AuthReply reply(m_storage->serverData(), user.data(), user->cookie(), result.authId, result.json);
//    packets.prepend(reply.data(m_sendStream));
//  }
//
//  packets.append(UserWriter(m_sendStream, user.data(), user->id(), UserWriter::StaticData).data());
//  send(QList<quint64>() << m_packetsEvent->socket(), packets, result.option, user->id());
//
//  if (m_plugins) {
//    UserHook hook(user);
//    m_plugins->hook(hook);
//  }
}


/*!
 * Отклонение авторизации.
 */
void Core::reject(const AuthResult &result)
{
  SCHAT_LOG_DEBUG(<< "REJECT AUTH" << result.status << Notice::status(result.status) << SimpleID::encode(result.authId));

  AuthReply reply(m_storage->serverData(), result.status, result.authId, result.json);

  NewPacketsEvent *event = new NewPacketsEvent(QList<quint64>() << m_packetsEvent->socket(), reply.data(m_sendStream));
  event->option = result.option;

  QCoreApplication::postEvent(m_listener, event);
}


bool Core::readUserData()
{
  ChatUser user = m_storage->user(m_reader->sender());
  if (!user)
    return false;

  UserReader reader(m_reader);
  if (updateUserData(user, &reader.user)) {
    route();
    return true;
  }

  return false;
}


/*!
 * Обновление данных о пользователе.
 */
bool Core::updateUserData(ChatUser user, User *other)
{
//  bool rename = false;

//  if (user->nick() != other->nick()) {
//    ChatUser u = m_storage->user(other->nick(), true);
//    if (u && u != user)
//      return false;
//
//    rename = true;
//  }
//
//  user->gender().setRaw(other->gender().raw());
//  if (rename) {
//    user->setNick(other->nick());
//    m_storage->rename(user);
//  }

  return true;
}


/*!
 * Обновление статуса пользователя.
 */
bool Core::updateUserStatus()
{
  ChatUser user = m_storage->user(m_reader->sender());
  if (!user)
    return true;

  if (!user->setStatus(m_messageData->text))
    return true;

  if (user->status() == Status::Offline)
    user->setStatus(Status::Online);

  return false;
}


/*!
 * Обработка физического отключения пользователя от сервера.
 */
void Core::release(SocketReleaseEvent *event)
{
  ChatChannel user = m_storage->channel(event->channelId(), SimpleID::UserId);
  if (!user)
    return;

  user->removeSocket(event->socket());

  NodeNoticeReader::release(user, event->socket());

  if (user->sockets().size())
    return;

  m_storage->remove(user);
}


/*!
 * Обработка команд.
 *
 * \return true в случае если команда была обработана, иначе false.
 * \deprecated Сервер больше не должен интерпретировать команды в сообщениях.
 */
bool Core::command()
{
  QString command = m_messageData->command;
  SCHAT_DEBUG_STREAM(this << "command()" << command << SimpleID::encode(m_packetsEvent->channelId()))

  if (command.isEmpty())
    return false;

  if (command == QLatin1String("status"))
    return updateUserStatus();

  if (command == QLatin1String("ready")) {
    userReadyHook();
    return true;
  }

  if (command == QLatin1String("topic"))
    return readTopic();

  return false;
}


/*!
 * Чтение пакета Protocol::MessagePacket.
 */
bool Core::message()
{
  MessageReader reader(m_reader);
  m_messageData = &reader.data;

  SCHAT_DEBUG_STREAM(this << "message()" << m_messageData->options);

  if (SimpleID::typeOf(m_reader->dest()) == SimpleID::UserId && !m_storage->user(m_reader->dest())) {
    rejectMessage(Notice::UserNotExists);
    return false;
  }

  if (m_messageData->options & MessageData::ControlOption && command())
    return true;

  m_timestamp = DateTime::utc();

  if (route()) {
    acceptMessage();
    return true;
  }
  else {
    rejectMessage(Notice::InternalError);
    return false;
  }
}


/*!
 * Чтение команды изменения топика.
 * Если топик был изменён функция возвратит false для того чтобы команда
 * была отослана дальше.
 */
bool Core::readTopic()
{
  return true;
//  if (SimpleID::typeOf(m_reader->dest()) != SimpleID::ChannelId)
//    return true;
//
//  ChatChannel channel = m_storage->channel(m_reader->dest());
//  if (!channel)
//    return true;
//
//  if (channel->topic().topic == m_messageData->text)
//    return true;
//
//  m_timestamp = Storage::timestamp();
//  channel->setTopic(m_messageData->text, m_reader->sender(), m_timestamp);
//  m_storage->update(channel);
//  return false;
}


/*!
 * Подтверждение доставки сообщения.
 */
void Core::acceptMessage(int status)
{
  if (m_messageData->id.isEmpty())
    return;

  if (status == Notice::UserOffline) {
    Notice notice(m_reader->dest(), m_reader->sender(), "msg.accepted", m_timestamp, m_messageData->id);
    notice.setStatus(status);

    QList<QByteArray> packets;
    packets.append(m_readBuffer);
    packets.append(notice.data(m_sendStream));
    send(m_storage->user(m_reader->sender()), packets);
  }

  acceptedMessageHook(status);
}


/*!
 * Отклонение сообщения.
 */
void Core::rejectMessage(int status)
{
  SCHAT_DEBUG_STREAM("rejectMessage()" << status)

  if (m_messageData->id.isEmpty())
    return;

  if (status == Notice::UserNotExists && m_plugins->has(NodeHook::OfflineDelivery)) {
    ChatUser user = m_storage->user(m_reader->dest(), true);
    if (user) {
      acceptMessage(Notice::UserOffline);
      return;
    }
  }

  Notice notice(m_reader->dest(), m_reader->sender(), "msg.rejected", m_timestamp, m_messageData->id);
  notice.setStatus(status);
  send(m_storage->user(m_reader->sender()), notice.data(m_sendStream));
}


/*!
 * Процедура авторизации анонимного пользователя во время работы.
 * В случае если пользователь успешно авторизирован, но текущий идентификатор
 * не совпадает с зарегистрированным, то пользователь будет принудительно отключен от сервера.
 *
 * В ответ клиенту высылается уведомление "login.reply"
 *
 * \sa Storage::login().
 */
bool Core::login()
{
  ChatUser user = m_storage->user(m_reader->sender());
  if (!user)
    return false;

  LoginReply reply = m_storage->login(user, m_notice->text(), m_reader->dest());
  Notice notice(m_reader->dest(), user->id(), "login.reply", DateTime::utc(), m_notice->id());
  notice.setStatus(reply.status());
  notice.setText(reply.name());

  int option = NewPacketsEvent::NoSocketOption;
  if (notice.status() == Notice::Conflict && !m_storage->isSlave(m_packetsEvent->channelId()))
    option = NewPacketsEvent::KillSocketOption;

  send(user, notice.data(m_sendStream), option);
  return true;
}


/*!
 * Обработка запроса на авторизацию.
 * В ответ клиенту высылается уведомление "reg.reply".
 * \todo Добавить корректное уведомление об ошибке если этот идентификатор пользователя уже зарегистрирован.
 */
bool Core::reg()
{
  ChatUser user = m_storage->user(m_reader->sender());
  if (!user)
    return false;

  RegReply reply = m_storage->reg(user, m_notice->text(), m_reader->dest(), m_notice->json());
  Notice notice(m_reader->dest(), user->id(), "reg.reply", DateTime::utc(), m_notice->id());
  notice.setStatus(reply.status());
  notice.setText(reply.name());

  send(user, notice.data(m_sendStream));
  if (reply.status() == Notice::OK)
    login();

  return true;
}


/*!
 * Чтение пакетов типа Protocol::NoticePacket.
 * \sa Notice.
 */
void Core::notice(quint16 type)
{
  if (type == Notice::GenericType) {
    Notice notice(type, m_reader);
    m_notice = &notice;

    if (!m_notice->isValid()) {
      rejectNotice(Notice::BadRequest);
      return;
    }

    QString command = m_notice->command();
    SCHAT_LOG_TRACE(<< "NOTICE PACKET" << command << notice.text() << notice.raw());

    if (command == "reg")
      reg();

    else if (command == "login")
      login();

//    else if (command == "leave")
//      leave(m_storage->channel(m_reader->sender()), m_packetsEvent->socket());

    else {
      route();
      return;
    }
  }
  else {
    if (NodeNoticeReader::read(type, m_reader))
      route();

    return;
  }

  route();
}


/*!
 * Отклонение входящего пакета Notice.
 *
 * \param status Причина отклонения, \sa Notice::StatusCodes.
 */
void Core::rejectNotice(int status)
{
  if (SimpleID::typeOf(m_notice->id()) != SimpleID::MessageId)
    return;

  ChatUser user = m_storage->user(m_reader->sender());
  if (!user)
    return;

  Notice notice(id(), m_reader->sender(), m_notice->command(), DateTime::utc(), m_notice->id());
  notice.setStatus(status);
  send(user, notice.data(m_sendStream));
}


void Core::acceptedMessageHook(int reason)
{
  if (!m_plugins)
    return;

  MessageHook hook(m_messageData, m_timestamp, reason);
  m_plugins->hook(hook);
}


void Core::userReadyHook()
{
  if (!m_plugins)
    return;

  ChatUser user = m_storage->user(m_reader->sender());
  if (!user)
    return;

  UserReadyHook hook(user, m_messageData->text, m_reader->dest());
  m_plugins->hook(hook);
}
