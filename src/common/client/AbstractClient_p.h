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

#ifndef ABSTRACTCLIENT_P_H_
#define ABSTRACTCLIENT_P_H_

#include <QHash>

#include "Channel.h"
#include "client/AbstractClient.h"
#include "net/SimpleSocket_p.h"

class AuthReply;
class PacketReader;
class NetworkPool;

class AbstractClientPrivate : public SimpleSocketPrivate
{
  Q_DECLARE_PUBLIC(AbstractClient);

public:
  AbstractClientPrivate();
  virtual ~AbstractClientPrivate();

  // Установка и завершение соединения.
  QString mangleNick();
  virtual bool authReply(const AuthReply &reply);
  virtual void restore() {}
  virtual void setClientState(AbstractClient::ClientState state);
  virtual void setup() {}
  void setServerData(const ServerData &data);
  void startReconnectTimer();

  AbstractClient::ClientState clientState;   ///< Состояние клиента.
  AbstractClient::ClientState previousState; ///< Предыдущее состояние клиента.
  bool sendLock;                           ///< Блокировка отправки пакетов, пакеты будут добавлены в очередь и будут отправлены после снятия блокировки.
  ClientUser user;                         ///< Пользователь.
  int reconnects;                          ///< Число попыток восстановить соединение.
  NetworkPool *pool;                       ///< Пул адресов серверов.
  PacketReader *reader;                    ///< Текущий объект PacketReader выполняющий чтение пакета.
  QBasicTimer *reconnectTimer;             ///< Таймер управляющий попытками переподключения.
  QByteArray cookie;                       ///< Cookie.
  QByteArray uniqueId;                     ///< Уникальный идентификатор пользователя.
  QList<QByteArray> sendQueue;             ///< Список виртуальных пакетов, ожидающих отправки если установлена блокировка на отправку.
  QString nick;                            ///< Оригинальный ник пользователя.
  QUrl url;                                ///< Адрес, к которому будет подключен клиент.
  ServerData *serverData;                  ///< Данные о сервере.
};

#endif /* ABSTRACTCLIENT_P_H_ */
