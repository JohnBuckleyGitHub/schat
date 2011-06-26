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
#include "net/packets/users.h"
#include "net/SimpleClient.h"
#include "User.h"

ChatSettings::ChatSettings(QObject *parent)
  : Settings(parent)
  , m_client(0)
{
  setDefault("Height", 420);
  setDefault("Maximized", false);
  setDefault("Width", 666);
  setDefault("WindowsAero", true);
  setDefault("Networks", QStringList());
  setDefault("ShowSeconds", false);
  setDefault("DefaultProfile", true);
  setDefault("Profile/Nick", User::defaultNick());
  setDefault("Profile/Gender", 0);
  setDefault("Profile/Status", User::OnlineStatus);
}


void ChatSettings::setClient(SimpleClient *client)
{
  m_client = client;

  m_user = m_client->user();
  m_user->setNick(value(ProfileNick).toString());
  m_user->setRawGender(value(ProfileGender).toUInt());
  m_user->setStatus(value(ProfileStatus).toUInt());

  connect(m_client, SIGNAL(userDataChanged(const QByteArray &)), SLOT(updateUserData(const QByteArray &)));
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

  setValue(ProfileNick, user->nick(), true);
  setValue(ProfileGender, user->rawGender(), true);
  setValue(ProfileStatus, user->status(), true);

  if (sync) {
    m_client->setNick(user->nick());
    m_user->setRawGender(user->rawGender());
  }
}


/*!
 * Обновление настройки профиля.
 */
void ChatSettings::updateValue(int key, const QVariant &value)
{
  if (this->value(key) == value)
    return;

  if (key == ProfileNick && m_client->clientState() != SimpleClient::ClientOnline) {
    setValue(key, value, true);
    m_client->setNick(value.toString());
    return;
  }

  User user(m_user.data());

  switch (key) {
    case ProfileNick:
      user.setNick(value.toString());
      break;

    case ProfileGender:
      user.setRawGender(value.toInt());
      break;

    default:
      break;
  }

  update(&user);
}


void ChatSettings::updateUserData(const QByteArray &userId)
{
  if (m_client->userId() != userId)
    return;

  update(m_user.data(), false);
}
