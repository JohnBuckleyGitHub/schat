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

#ifndef CLIENTHELPER_H_
#define CLIENTHELPER_H_

#include <QObject>
#include <QStringList>

#include "schat.h"

class ClientCmd;
class MessageData;
class SimpleClient;

/*!
 * Дополнительный слой абстракции между клиентом и приложением.
 */
class SCHAT_EXPORT ClientHelper : public QObject
{
  Q_OBJECT

public:
  ClientHelper(SimpleClient *client);
  bool send(MessageData &data);
  inline SimpleClient *client() { return m_client; }
  virtual bool sendText(MessageData &data);
  virtual int command(MessageData &data, const QString &cmd, const QString &text);
  virtual void command(const ClientCmd &cmd);

protected:
  bool m_richText;        ///< true если в командах может использоваться html текст.
  QByteArray m_destId;    ///< Текущий получатель сообщения.
  QStringList m_commands; ///< Список специальных команд таких как "/me".
  quint64 m_name;         ///< Счётчик последнего отправленного сообщения.
  SimpleClient *m_client; ///< Указатель на клиент.
};

#endif /* CLIENTHELPER_H_ */
