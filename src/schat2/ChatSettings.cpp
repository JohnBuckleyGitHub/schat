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

#include "ChatCore.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/SimpleClient.h"
#include "net/packets/messages.h"
#include "net/packets/users.h"
#include "NetworkManager.h"
#include "ui/ChannelUtils.h"
#include "ui/UserUtils.h"
#include "User.h"

ChatSettings::ChatSettings(const QString &fileName, QObject *parent)
  : Settings(fileName, parent)
{
  setDefault(QLatin1String("AutoConnect"),         true);
  setDefault(QLatin1String("ChannelUserCount"),    false);
  setDefault(QLatin1String("CookieAuth"),          true);
  setDefault(QLatin1String("DeveloperExtras"),     false);
  setDefault(QLatin1String("Height"),              420);
  setDefault(QLatin1String("HideIgnore"),          true);
  setDefault(QLatin1String("Maximized"),           false);
  setDefault(QLatin1String("Networks"),            QStringList());
  setDefault(QLatin1String("ShowSeconds"),         false);
  setDefault(QLatin1String("ShowServiceMessages"), false);
  setDefault(QLatin1String("Translation"),         QLatin1String("auto"));
  setDefault(QLatin1String("Width"),               666);
  setDefault(QLatin1String("WindowsAero"),         true);
  setDefault(QLatin1String("Profile/Gender"),      0);
  setDefault(QLatin1String("Profile/Nick"),        User::defaultNick());
  setDefault(QLatin1String("Profile/Status"),      1);
}


void ChatSettings::setClient(SimpleClient *client)
{
  m_client = client;

  m_channel = ChatClient::channel();
  m_channel->setName(value("Profile/Nick").toString());
  m_channel->gender().setRaw(value("Profile/Gender").toInt());
  m_channel->status().set(value("Profile/Status").toInt());

  connect(m_client, SIGNAL(userDataChanged(const QByteArray &)), SLOT(updateUserData(const QByteArray &)));
}


/*!
 * Обновление настройки профиля.
 */
void ChatSettings::updateValue(const QString &key, const QVariant &value)
{
//  if (this->value(key) == value)
//    return;
//
//  if (key == QLatin1String("Profile/Nick") && m_client->clientState() != SimpleClient::ClientOnline) {
//    setValue(key, value);
//    m_client->setNick(value.toString());
//    return;
//  }
//
//  if (key == QLatin1String("Profile/Status")) {
//    updateStatus(value);
//    return;
//  }
//
//  User user(m_user.data());
//
//  if (key == QLatin1String("Profile/Nick"))
//    user.setNick(value.toString());
//  else if (key == QLatin1String("Profile/Gender"))
//    user.gender().setRaw(value.toInt());
//
//  update(&user);
}


void ChatSettings::updateUserData(const QByteArray &userId)
{
  if (m_client->channelId() != userId)
    return;

//  update(m_user.data(), false);
}


/*!
 * Обновление информации о пользователе.
 * В случае если клиент подключен к серверу и \p sync = true, происходит отложенная синхронизация.
 *
 * \param user Указатель на пользователя с новыми данными.
 * \param sync true Если необходимо синхронизировать изменения с сервером.
 */
void ChatSettings::update(User *user, bool sync)
{
  if (sync && m_client->clientState() == SimpleClient::ClientOnline) {
    UserWriter writer(m_client->sendStream(), user);
    m_client->send(writer.data());
    return;
  }

  if (sync) {
    m_client->setNick(user->nick());
//    m_user->gender().setRaw(user->gender().raw());
//    m_user->setStatus(user->statusToString());
  }

  setValue(QLatin1String("Profile/Nick"), m_client->nick());
  setValue(QLatin1String("Profile/Gender"), user->gender().raw());
  setValue(QLatin1String("Profile/Status"), user->statusToString());
}


/*!
 * Обновление статуса пользователя.
 *
 * \param value Новый статус, может быть числом или строкой.
 */
void ChatSettings::updateStatus(const QVariant &value)
{
  QString status;
//  if (value.type() == QVariant::Int)
//    status = User::statusToString(value.toInt(), m_user->statusText(value.toInt()));
//  else
//    status = value.toString();

  if (m_client->clientState() == SimpleClient::ClientOnline) {
//    MessageData data(UserUtils::userId(), m_client->user()->channels(), QLatin1String("status"), status);
//    m_client->send(data, true);
  }

//  m_user->setStatus(status);
//  if (m_user->status() == ::Status::Offline) {
//    m_client->leave();
//  }

  setValue(QLatin1String("Profile/Status"), status);

  ChatCore::i()->networks()->open();
}
