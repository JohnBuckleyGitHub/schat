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

#ifndef MESSAGEADAPTER_H_
#define MESSAGEADAPTER_H_

#include <QHash>
#include <QObject>

class MessageData;
class NoticeData;
class SimpleClient;

class MessageAdapter : public QObject
{
  Q_OBJECT

public:
  /// Результат возвращаемый функцией отправки сообщения.
  enum SendResult {
    NoSent,
    ErrorTextEmpty,
    SentAsText,
    SentAsCommand,
    CommandArgsError
  };

  MessageAdapter(SimpleClient *client);
  int send(MessageData &data);

signals:
  void message(int status, const MessageData &data);

private slots:
  void allDelivered(quint64 id);
  void clientMessage(const MessageData &data);
  void clientStateChanged(int state);
  void notice(const NoticeData &data);

private:
  bool sendCommand(MessageData &data);
  bool sendText(MessageData &data);
  int setGender(const QString &gender, const QString &color);
  void command(const QString &text);
  void setStateAll(int state, const QString &reason);

  bool m_richText;                          ///< true если в командах может использоваться html текст.
  QHash<quint64, QByteArray> m_undelivered; ///< Таблица сообщений доставка которых не подтверждена.
  quint64 m_name;                           ///< Счётчик последнего отправленного сообщения.
  SimpleClient *m_client;                   ///< Указатель на клиент.
};

#endif /* MESSAGEADAPTER_H_ */
