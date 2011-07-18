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

#ifndef CHATSETTINGS_H_
#define CHATSETTINGS_H_

#include "Settings.h"
#include "User.h"

class SimpleClient;

class ChatSettings : public Settings
{
  Q_OBJECT

public:
  /// Ключи настроек.
  enum Keys {
    AutoConnect,    ///< Автоматически подключатся.
    Height,         ///< Высота окна.
    Maximized,      ///< Окно развёрнуто на весь экран.
    Networks,       ///< Список серверов.
    ShowSeconds,    ///< Отображать секунды.
    Translation,    ///< Перевод.
    Width,          ///< Ширина окна.
    WindowsAero,    ///< При доступности будет использован интерфейс Windows Aero.
    ProfileNick,    ///< Ник по умолчанию.
    ProfileGender,  ///< Пол по умолчанию.
    ProfileStatus   ///< Статус пользователя.
  };

  ChatSettings(QObject *parent = 0);
  void setClient(SimpleClient *client);
  void updateValue(int key, const QVariant &value);

private slots:
  void updateUserData(const QByteArray &userId);

private:
  void update(User *user, bool sync = true);
  void updateStatus(const QVariant &value);

  ClientUser m_user;
  SimpleClient *m_client;
};

#endif /* CHATSETTINGS_H_ */
