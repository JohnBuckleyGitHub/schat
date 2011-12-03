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

#ifndef CLIENTMESSAGES_H_
#define CLIENTMESSAGES_H_

#include <QObject>

#include "schat.h"

class MessagePacket;
class SimpleClient;

namespace Hooks
{
  class Messages;
}

class SCHAT_EXPORT ClientMessages : public QObject
{
  Q_OBJECT

public:
  ClientMessages(QObject *parent = 0);
  QByteArray randomId() const;

  inline void setHooks(Hooks::Messages *hooks) { m_hooks = hooks; }

  bool send(const QByteArray &dest, const QString &text);
  bool sendText(const QByteArray &dest, const QString &text);

private slots:
  void notice(int type);

private:
  Hooks::Messages *m_hooks; ///< Хуки.
  MessagePacket *m_packet;  ///< Текущий прочитанный пакет.
  QByteArray m_destId;      ///< Текущий получатель сообщения.
  SimpleClient *m_client;   ///< Клиент чата.
};

#endif /* CLIENTMESSAGES_H_ */
