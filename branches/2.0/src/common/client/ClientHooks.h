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

#ifndef CLIENTHOOKS_H_
#define CLIENTHOOKS_H_

#include <QObject>

#include "schat.h"

class ClientCmd;
class MessagePacket;

namespace Hooks
{

class SCHAT_EXPORT Messages : public QObject
{
  Q_OBJECT

public:
  Messages(QObject *parent = 0);
  virtual bool command(const QByteArray &dest, const ClientCmd &cmd);
  virtual void readText(const MessagePacket &packet) { Q_UNUSED(packet) }
  virtual void sendText(const MessagePacket &packet) { Q_UNUSED(packet) }
};

}  // namespace Hooks

#endif /* CLIENTHOOKS_H_ */
