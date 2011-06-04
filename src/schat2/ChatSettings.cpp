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
{
  setDefault("Height", 420);
  setDefault("Maximized", false);
  setDefault("Width", 666);
  setDefault("WindowsAero", true);
  setDefault("Networks", QStringList());
  setDefault("DefaultProfile", true);
  setDefault("Profile/Nick", User::defaultNick());
  setDefault("Profile/Gender", 0);
}


/*!
 * Обновление настройки профиля.
 */
void ChatSettings::updateValue(int key, const QVariant &value)
{
  SimpleClient *client = ChatCore::i()->client();

  if (client->clientState() != SimpleClient::ClientOnline) {
    setValue(key, value, true);
    return;
  }

  if (this->value(key) == value)
    return;

  switch (key) {
    case ProfileNick:
      updateNick(client, value.toString());
      break;

    default:
      break;
  }
}


void ChatSettings::updateNick(SimpleClient *client, const QString &nick)
{
  if (client->user()->nick() != nick && User::isValidNick(nick)) {
    User user(client->user());
    user.setNick(nick);
    UserWriter writer(client->sendStream(), &user);
    client->send(writer.data());
  }
}
