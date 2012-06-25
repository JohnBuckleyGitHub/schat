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

#include <QDebug>

#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientCmd.h"
#include "client/ClientHooks.h"
#include "client/ClientMessages.h"
#include "client/SimpleClient.h"
#include "DateTime.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "text/PlainTextFilter.h"

ClientMessages::ClientMessages(QObject *parent)
  : QObject(parent)
  , m_hooks(0)
  , m_client(ChatClient::io())
{
  m_hooks = new Hooks::Messages(this);

  connect(m_client, SIGNAL(notice(int)), SLOT(notice(int)));
  connect(ChatClient::channels(), SIGNAL(channels(const QList<QByteArray> &)), SLOT(channels(const QList<QByteArray> &)));
}


QByteArray ClientMessages::randomId() const
{
  return SimpleID::randomId(SimpleID::MessageId, m_client->channelId());
}


/*!
 * Возвращает \b true если используется время клиента, вместо времени пакета
 * для определения времени сообщения.
 *
 * \p status Статус пакета.
 */
bool ClientMessages::isClientDate(int status)
{
  if (status == Notice::Found || status == Notice::Unread || status == Notice::Read)
    return false;

  return true;
}


/*!
 * Отправка текстового сообщения, если в тексте будут команды, то они будут обработаны.
 */
bool ClientMessages::send(const QByteArray &dest, const QString &text)
{
  if (text.isEmpty())
    return false;

  m_destId = dest;
  QString plain = PlainTextFilter::filter(text);

  if (!plain.startsWith(LC('/')))
    return sendText(dest, text);

  if (m_hooks->command(dest, text, plain))
    return true;

  /// Для обработки обычных команд используется хук: Hooks::Messages::command(const QByteArray &dest, const ClientCmd &cmd).
  QStringList commands = (LS(" ") + plain).split(LS(" /"), QString::SkipEmptyParts);
  int matches = 0;
  for (int i = 0; i < commands.size(); ++i) {
    ClientCmd cmd(commands.at(i));
    if (cmd.isValid() && m_hooks->command(dest, cmd))
      matches++;
  }

  if (!matches)
    return sendText(dest, text);;

  return true;
}


/*!
 * Отправка текстового сообщения, команды не обрабатываются.
 */
bool ClientMessages::sendText(const QByteArray &dest, const QString &text, const QString &command)
{
  if (SimpleID::typeOf(dest) == SimpleID::ServerId)
    return false;

  MessagePacket packet(new MessageNotice(ChatClient::id(), dest, text, DateTime::utc(), randomId()));
  if (!command.isEmpty())
    packet->setCommand(command);

  if (m_client->send(packet, true)) {
    m_hooks->sendText(packet);

    return true;
  }

  return false;
}


void ClientMessages::insert(MessageNotice *notice)
{
  readText(MessagePacket(notice));
}


void ClientMessages::channels(const QList<QByteArray> &channels)
{
  foreach (QByteArray id, channels) {
    if (m_pending.contains(id)) {
      QList<MessagePacket> packets = m_pending.value(id);

      for (int i = 0; i < packets.size(); ++i)
        m_hooks->readText(packets.at(i));

      m_pending.remove(id);
    }
  }
}


/*!
 * Чтение входящего сообщения.
 */
void ClientMessages::notice(int type)
{
  if (type != Notice::MessageType)
    return;

  readText(MessagePacket(new MessageNotice(type, ChatClient::io()->reader())));
}


void ClientMessages::readText(MessagePacket packet)
{
  if (!packet->isValid())
    return;

  if (isClientDate(packet->status()))
    packet->setDate(ChatClient::date());

  /// В случае если отправитель сообщения неизвестен клиенту, то будет произведён вход в канал
  /// этого пользователя для получения информации о нём, само сообщения будет добавлено в очередь
  /// до момента получения информации об отправителе.
  ClientChannel user = ChatClient::channels()->get(packet->sender());

  if (ChatClient::state() == ChatClient::Online && (!user || !user->isSynced())) {
    if (!m_pending.contains(packet->sender()))
      ChatClient::channels()->join(packet->sender());

    m_pending[packet->sender()].append(packet);

    if (!user)
      return;
  }
  else if (!user)
    return;

  m_hooks->readText(packet);
}
