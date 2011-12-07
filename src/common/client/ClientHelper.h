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
#include <QVariant>

#include "schat.h"

class ClientCmd;
class MessageData;
class Notice;
class SimpleClient;

/*!
 * Дополнительный слой абстракции между клиентом и приложением.
 *
 * \deprecated Этот класс должен быть заменён на ClientMessages.
 */
class SCHAT_EXPORT ClientHelper : public QObject
{
  Q_OBJECT

public:
  ClientHelper(SimpleClient *client);
  inline SimpleClient *client() { return m_client; }
  QByteArray login(const QString &command, const QString &name, const QString &password, const QVariantMap &json = QVariantMap());
  QByteArray randomId() const;
  static qint64 timestamp();
  virtual bool sendText(MessageData &data);

signals:
  void loggedIn(const QString &name);
  void registered(const QString &name, const QByteArray &password);

protected slots:
  void notice(const Notice &notice);

protected:
  virtual bool loginReply(const Notice &notice);
  virtual bool regReply(const Notice &notice);
  virtual void notice() {}

  bool m_richText;        ///< true если в командах может использоваться html текст.
  const Notice *m_notice; ///< Текущий прочитанный объект Notice.
  QByteArray m_destId;    ///< Текущий получатель сообщения.
  QStringList m_commands; ///< Список специальных команд таких как "/me".
  SimpleClient *m_client; ///< Указатель на клиент.
};

#endif /* CLIENTHELPER_H_ */
