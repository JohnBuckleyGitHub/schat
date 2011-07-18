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

#include "client/ClientHelper.h"

class AbstractMessage;
class ChatSettings;
class NoticeData;
class SimpleClient;

class MessageAdapter : public ClientHelper
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

  MessageAdapter();
  bool sendText(MessageData &data);
  void command(const ClientCmd &cmd);

signals:
  void message(const AbstractMessage &message);

private slots:
  void allDelivered(quint64 id);
  void clientMessage(const MessageData &data);
  void clientStateChanged(int state);
  void notice(const NoticeData &data);

private:
  int setGender(const QString &gender, const QString &color);
  void newUserMessage(int status, const MessageData &data);
  void setStateAll(int state, const QString &reason);
  void setStatus(int status, const QString &text = "");

  ChatSettings *m_settings;                 ///< Настройки чата.
  QHash<quint64, QByteArray> m_undelivered; ///< Таблица сообщений доставка которых не подтверждена.
};

#endif /* MESSAGEADAPTER_H_ */
