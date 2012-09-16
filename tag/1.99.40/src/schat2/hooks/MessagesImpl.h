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

#ifndef MESSAGESIMPL_H_
#define MESSAGESIMPL_H_

#include "client/ClientHooks.h"
#include "net/packets/MessageNotice.h"

namespace Hooks
{

class SCHAT_CORE_EXPORT MessagesImpl : public Messages
{
  Q_OBJECT

public:
  MessagesImpl(QObject *parent = 0);
  int readText(MessagePacket packet);
  void sendText(MessagePacket packet);

protected:
  virtual void unhandled(MessagePacket packet) const;

private slots:
  void clientStateChanged(int state, int previousState);

private:
  QHash<QByteArray, MessagePacket> m_undelivered; ///< Таблица сообщений доставка которых не подтверждена.
};

} // namespace Hooks

#endif /* MESSAGESIMPL_H_ */
