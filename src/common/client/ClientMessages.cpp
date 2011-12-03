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

#include "client/ChatClient.h"
#include "client/ClientCmd.h"
#include "client/ClientHooks.h"
#include "client/ClientMessages.h"
#include "client/SimpleClient.h"
#include "DateTime.h"
#include "net/packets/MessagePacket.h"
#include "net/SimpleID.h"
#include "text/PlainTextFilter.h"

ClientMessages::ClientMessages(QObject *parent)
  : QObject(parent)
  , m_hooks(0)
  , m_client(ChatClient::io())
{
  connect(m_client, SIGNAL(notice(int)), SLOT(notice(int)));
}


QByteArray ClientMessages::randomId() const
{
  return SimpleID::randomId(SimpleID::MessageId, m_client->channelId());
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

  if (!plain.startsWith('/'))
    return sendText(dest, text);

  if (!m_hooks)
    return false;

  QStringList commands = (" " + plain).split(" /", QString::SkipEmptyParts);
  for (int i = 0; i < commands.size(); ++i) {
    ClientCmd cmd(commands.at(i));
    if (cmd.isValid())
      m_hooks->command(dest, cmd);
  }

  return true;
}


/*!
 * Отправка текстового сообщения, команды не обрабатываются.
 */
bool ClientMessages::sendText(const QByteArray &dest, const QString &text)
{
  MessagePacket packet(ChatClient::id(), dest, text, DateTime::utc(), randomId());
  if (m_client->send(packet, true)) {
    if (m_hooks)
      m_hooks->sendText(packet);

    return true;
  }

  return false;
}


/*!
 * Чтение входящего сообщения.
 */
void ClientMessages::notice(int type)
{
  if (type != Notice::MessageType)
    return;

  MessagePacket packet(type, ChatClient::io()->reader());
  if (!packet.isValid())
    return;

  qDebug() << ">>>" << packet.text();

  m_packet = &packet;
  m_packet->setDate(ChatClient::io()->date());

  if (m_hooks)
    m_hooks->readText(packet);
}
