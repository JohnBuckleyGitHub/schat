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

class SimpleClient;

class ChatSettings : public Settings
{
  Q_OBJECT

public:
  /// Ключи настроек.
  enum Keys {
    Height,         ///< Высота окна.
    Maximized,      ///< Окно развёрнуто на весь экран.
    Width,          ///< Ширина окна.
    WindowsAero,    ///< При доступности будет использован интерфейс Windows Aero.
    Networks,       ///< Список серверов.
    DefaultProfile, ///< Использовать профиль по умолчанию.
    ProfileNick,    ///< Ник по умолчанию.
    ProfileGender   ///< Пол по умолчанию.
  };

  ChatSettings(QObject *parent = 0);
  void updateValue(int key, const QVariant &value);

private:
  void updateNick(SimpleClient *client, const QString &nick);
};

#endif /* CHATSETTINGS_H_ */
