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

#ifndef CHATCLIENT_H_
#define CHATCLIENT_H_

#include <QObject>

#include "Channel.h"

class ClientChannels;
class ClientMessages;
class SimpleClient;

namespace Hooks
{
  class Client;
}

class SCHAT_EXPORT ChatClient : public QObject
{
  Q_OBJECT

public:
  ChatClient(QObject *parent = 0);
  inline Hooks::Client *hooks() const { return m_hooks; }

  inline static ChatClient *i()            { return m_self; }
  inline static ClientChannel channel()    { return m_self->getChannel(); }
  inline static ClientChannels *channels() { return m_self->m_channels; }
  inline static ClientMessages *messages() { return m_self->m_messages; }
  inline static QByteArray id()            { return m_self->getId(); }
  inline static QByteArray serverId()      { return m_self->getServerId(); }
  inline static SimpleClient *io()         { return m_self->m_client; }

private:
  ClientChannel getChannel();
  QByteArray getId();
  QByteArray getServerId();

  ClientChannels *m_channels; ///< Каналы.
  ClientMessages *m_messages; ///< Обработчик сообщений.
  Hooks::Client *m_hooks;     ///< Хуки.
  SimpleClient *m_client;     ///< Клиент чата.
  static ChatClient *m_self;  ///< Указатель на себя.
};

#endif /* CHATCLIENT_H_ */
