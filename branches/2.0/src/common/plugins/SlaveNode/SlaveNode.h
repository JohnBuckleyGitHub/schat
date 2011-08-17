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

#ifndef SLAVENODE_H_
#define SLAVENODE_H_

#include "cores/Core.h"

class AbstractClient;
class MessageData;

class SlaveNode : public Core
{
  Q_OBJECT

public:
  /// Режим работы сервера.
  enum Mode {
    ProxyMode,   ///< Нормальный режим, соедениние с корневым сервером установлено.
    FailbackMode ///< Отсутствует соединение с корневым сервером.
  };

  SlaveNode(QObject *parent = 0);
  inline AbstractClient *uplink() { return m_uplink; }
  inline Mode mode() const { return m_mode; }
  int start();

protected:
  void readPacket(int type);
  bool readMessage();

private slots:
  void uplinkAuth();
  void uplinkPacketReady(int type);
  void uplinkReady();

private:
  bool uplinkRoute();
  bool uplinkRouteChannel(const QByteArray &id);
  bool uplinkRouteUser(const QByteArray &id);
  void setMode(Mode mode);
  void uplinkAuthReply();
  void uplinkReadChannel();
  void uplinkReadMessage();
  void uplinkRejectMessage(const MessageData &msg, int reason);

  AbstractClient *m_uplink; ///< Подключение к корневому серверу.
  Mode m_mode;              ///< Режим работы сервера.
};

#endif /* SLAVECORE_H_ */
