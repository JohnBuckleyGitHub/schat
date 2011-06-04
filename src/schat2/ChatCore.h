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

#ifndef CHATCORE_H_
#define CHATCORE_H_

#include <QIcon>
#include <QObject>
#include <QVariant>

class ChatSettings;
class MessageAdapter;
class MessageData;
class NetworkManager;
class SimpleClient;
class UserUtils;

#define SCHAT_ICON(x) ChatCore::icon(ChatCore::x)
#define SCHAT_OPTION(x) ChatCore::i()->settings()->value(ChatSettings::x)

class ChatCore : public QObject
{
  Q_OBJECT

public:
  /// Иконки.
  enum IconName {
    ChannelIcon,        ///< Иконка канала.
    GearIcon,           ///< Иконка в виде шестерёнки.
    MainTabMenuIcon,    ///< Иконка главного меню вкладок.
    NetworkErrorIcon,   ///< Критическая ошибка подключения.
    NetworkOfflineIcon, ///< Нет подключения.
    NetworkOnlineIcon,  ///< Подключение установлено.
    ConnectIcon,        ///< Подключение.
    DisconnectIcon,     ///< Отключение.
    QuitIcon,           ///< Выход.
    SecureIcon,         ///< Шифрование.
    SettingsIcon,       ///< Настройки.
    SoundIcon,
    SoundMuteIcon,
    UsersIcon,
    SmallLogoIcon,
    SmallLogoNYIcon,
    TextBoldIcon,
    TextItalicIcon,
    TextStrikeIcon,
    TextUnderlineIcon,
    SendIcon,
    GlobeIcon,
    AddIcon,
    RemoveIcon,
    ProfileIcon,
    OkIcon
  };

  enum Notice {
    AboutNotice,
    QuitNotice,
    SettingsNotice,
    ToggleVisibilityNotice,
    NetworkChangedNotice
  };

  ChatCore(QObject *parent = 0);
  ~ChatCore();
  inline ChatSettings *settings() { return m_settings; }
  inline NetworkManager *networks() const { return m_networkManager; }
  inline SimpleClient *client() { return m_client; }
  inline static ChatCore *i() { return m_self; }
  inline void startNotify(int notice, const QVariant &data = QVariant()) { emit notify(notice, data); }
  static QIcon icon(IconName name);
  void send(const QByteArray &destId, const QString &text);

signals:
  void message(int status, const MessageData &data);
  void notify(int notice, const QVariant &data);

private:
  ChatSettings *m_settings;         ///< Настройки.
  MessageAdapter *m_messageAdapter; ///< Адаптер отправки и получения сообщений.
  NetworkManager *m_networkManager; ///< Объект управляющих сетями.
  SimpleClient *m_client;           ///< Клиент.
  static ChatCore *m_self;          ///< Указатель на себя.
  static QStringList m_icons;       ///< Иконки.
  UserUtils *m_userUtils;           ///< Утилиты манипуляции над пользователем.
};

#endif /* CHATCORE_H_ */
