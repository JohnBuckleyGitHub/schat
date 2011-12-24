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

#include <QTextDocument>

#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientMessages.h"
#include "client/SimpleClient.h"
#include "DateTime.h"
#include "messages/ServiceMessage.h"
#include "net/SimpleID.h"
#include "NetworkManager.h"

ServiceMessage::ServiceMessage(const QString &text, const QByteArray &user)
  : Message()
{
  m_data["Type"] = "service";
  m_data["Id"]   = SimpleID::encode(ChatClient::messages()->randomId());
  m_data["Text"] = text;

  qint64 date = ChatClient::io()->date();
  if (!date)
    date = DateTime::utc();

  m_data["Date"] = date;

  author(user);
}


bool ServiceMessage::isValid() const
{
  if (m_data.value("Text").toString().isEmpty())
    return false;

  return true;
}


ServiceMessage ServiceMessage::connected()
{
  ServiceMessage message(QObject::tr("Successfully connected to <b>%1</b>").arg(Qt::escape(NetworkManager::currentServerName())));
  message.data()["Type"]  = "info";
  message.data()["Extra"] = "green-text";
  return message;
}


ServiceMessage ServiceMessage::connectionLost()
{
  ServiceMessage message(QObject::tr("Connection lost"));
  message.data()["Type"]  = "info";
  message.data()["Extra"] = "red-text";
  return message;
}


/*!
 * Сообщение о входе пользователя в канал.
 */
ServiceMessage ServiceMessage::joined(const QByteArray &user)
{
  QString text;
  ClientChannel channel = ChatClient::channels()->get(user);

  if (channel) {
    if (channel->gender().value() == Gender::Female)
      text = QObject::tr("has joined", "Female");
    else
      text = QObject::tr("has joined", "Male");
  }

  return ServiceMessage(text, user);
}


/*!
 * Сообщение о выходе пользователя из канала.
 */
ServiceMessage ServiceMessage::part(const QByteArray &user)
{
  QString text;
  ClientChannel channel = ChatClient::channels()->get(user);

  if (channel) {
    if (channel->gender().value() == Gender::Female)
      text = QObject::tr("has left", "Female");
    else
      text = QObject::tr("has left", "Male");
  }

  return ServiceMessage(text, user);
}


/*!
 * Сообщение о выходе пользователя из чата.
 */
ServiceMessage ServiceMessage::quit(const QByteArray &user)
{
  QString text;
  ClientChannel channel = ChatClient::channels()->get(user);

  if (channel) {
    if (channel->gender().value() == Gender::Female)
      text = QObject::tr("has quit chat", "Female");
    else
      text = QObject::tr("has quit chat", "Male");
  }

  return ServiceMessage(text, user);
}
