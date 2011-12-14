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
#include "net/packets/messages.h"

class AbstractMessage;
class AbstractNotice;
class ChatSettings;
class Notice;
class SimpleClient;

class SCHAT_CORE_EXPORT MessageAdapter : public ClientHelper
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
  void command(const ClientCmd &cmd);

signals:
  void channelDataChanged(const QByteArray &senderId, const QByteArray &channelId);
  void message(const AbstractMessage &message);

protected:
  void notice();

private slots:
  void message(const MessageData &data);
  void clientStateChanged(int state, int previousState);

private:
  int setGender(const QString &gender, const QString &color);
  MessageData msgFromNotice() const;
  void commandHelpHint(const QString &command);
  void newUserMessage(int status, const MessageData &data);
  void readTopic(const MessageData &data);
  void setStateAll(int state);
  void setStatus(int status, const QString &text = QString());

  ChatSettings *m_settings;                     ///< Настройки чата.
  QHash<QByteArray, MessageData> m_undelivered; ///< Таблица сообщений доставка которых не подтверждена.
};

#endif /* MESSAGEADAPTER_H_ */
