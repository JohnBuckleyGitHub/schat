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

#include "client/AbstractClient.h"
#include "net/SimpleSocket_p.h"

class PacketReader;
class SyncChannelCache;


class AbstractClientPrivate : public SimpleSocketPrivate
{
  Q_DECLARE_PUBLIC(AbstractClient);

public:
  AbstractClientPrivate();
  virtual ~AbstractClientPrivate();

  // Установка и завершение соединения.
  bool readAuthReply();
  QString mangleNick();
  virtual void restore() {}
  virtual void setup() {}
  void setClientState(AbstractClient::ClientState state);
  void setServerData(const ServerData &data);

  // Каналы.
  bool addChannel(Channel *channel);
  bool readChannel();
  void endSyncChannel(Channel *channel);
  void endSyncChannel(const QByteArray &id);

  // Сообщения.
  bool command();
  bool readMessage();

  bool readNotice();

  // Пользователи.
  bool readUserData();
  bool removeUser(const QByteArray &userId);
  bool removeUserFromChannel(const QByteArray &channelId, const QByteArray &userId, bool clear = true);
  void updateUserData(ClientUser existUser, User *user);
  void updateUserStatus(const QString &text);

  bool sendLock;                           ///< Блокировка отправки пакетов, пакеты будут добавлены в очередь и будут отправлены после снятия блокировки.
  ClientUser user;                         ///< Пользователь.
  int reconnects;                          ///< Число попыток восстановить соединение.
  MessageData *messageData;                ///< Текущий прочитанный объект MessageData.
  PacketReader *reader;                    ///< Текущий объект PacketReader выполняющий чтение пакета.
  QBasicTimer *reconnectTimer;             ///< Таймер управляющий попытками переподключения.
  QByteArray uniqueId;                     ///< Уникальный идентификатор пользователя.
  QHash<QByteArray, Channel*> channels;    ///< Таблица каналов.
  QHash<QByteArray, ClientUser> users;     ///< Таблица пользователей.
  QList<QByteArray> sendQueue;             ///< Список виртуальных пакетов, ожидающих отправки если установлена блокировка на отправку.
  QString nick;                            ///< Оригинальный ник пользователя.
  QUrl url;                                ///< Адрес, к которому будет подключен клиент.
  ServerData *serverData;                  ///< Данные о сервере.
  AbstractClient::ClientState clientState;   ///< Состояние клиента.
  AbstractClient::ClientState previousState; ///< Предыдущее состояние клиента.
};

#endif /* ABSTRACTCLIENT_P_H_ */
