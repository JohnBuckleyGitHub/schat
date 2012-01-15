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

#ifndef CLIENTMESSAGES_H_
#define CLIENTMESSAGES_H_

#include <QObject>
#include <QHash>

#include "net/packets/MessageNotice.h"

class MessageNotice;
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
  inline Hooks::Messages *hooks() const { return m_hooks; }
  QByteArray randomId() const;

  bool send(const QByteArray &dest, const QString &text);
  bool sendText(const QByteArray &dest, const QString &text, const QString &command = QString());

private slots:
  void channels(const QList<QByteArray> &channels);
  void notice(int type);

private:
  Hooks::Messages *m_hooks;                           ///< Хуки.
  MessagePacket m_packet;                            ///< Текущий прочитанный пакет.
  QByteArray m_destId;                                ///< Текущий получатель сообщения.
  QHash<QByteArray, QList<MessagePacket> > m_pending; ///< Сообщения отображение которых отложена, т.к. не известна информация об отправителе.
  SimpleClient *m_client;                             ///< Клиент чата.
};

#endif /* CLIENTMESSAGES_H_ */
