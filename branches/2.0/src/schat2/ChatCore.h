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
#include <QUrl>
#include <QVariant>

#include "schat.h"

class AbstractMessage;
class ChatCorePrivate;
class ChatPlugins;
class ChatSettings;
class ChatViewAction;
class FileLocations;
class MessageAdapter;
class MessageData;
class NetworkManager;
class SimpleClient;
class StatusMenu;
class Translation;
class UserUtils;

#define SCHAT_ICON(x) ChatCore::icon(ChatCore::x)
#define SCHAT_OPTION(x) ChatCore::i()->settings()->value(QLatin1String(x))

class SCHAT_CORE_EXPORT ChatCore : public QObject
{
  Q_OBJECT

public:
  /// Иконки.
  enum IconName {
    ChannelIcon,        ///< Иконка канала.
    ChannelAlertIcon,
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
    OkIcon,
    InfoBalloon,
    EditClear,
    EditCopy,
    EditCut,
    EditPaste,
    EditSelectAll,
    Balloon,
    Slash,
    TopicEdit
  };

  enum Notice {
    AboutNotice,
    QuitNotice,
    SettingsNotice,
    ToggleVisibilityNotice,
    NetworkChangedNotice,
    InsertTextToSend,       ///< Запрос на вставку текста в поле отравки.
    AddPrivateTab,          ///< Добавление приватного разговора и установка фокуса на него.
    ShowChatNotice,         ///< Запрос на открытие и активацию окна чата.
    EditTopicNotice,        ///< Редактирование темы канала, параметр: идентификатор канала.
    SetSendFocusNotice      ///< Установка фокуса на поле отправки сообщения.
  };

  ChatCore(QObject *parent = 0);
  ~ChatCore();
  bool isIgnored(const QByteArray &id);
  inline ChatPlugins *plugins() { return m_plugins; }
  inline ChatSettings *settings() { return m_settings; }
  inline FileLocations *locations() const { return m_locations; }
  inline NetworkManager *networks() const { return m_networkManager; }
  inline QByteArray currentId() const { return m_currentId; }
  inline SimpleClient *client() { return m_client; }
  inline static ChatCore *i() { return m_self; }
  inline StatusMenu *statusMenu() { return m_statusMenu; }
  inline Translation *translation() { return m_translation; }
  inline void addChatViewAction(const QString &id, ChatViewAction *action) { m_actions.insert(id, action); }
  inline void setCurrentId(const QByteArray &id) { m_currentId = id; }
  inline void setStatusMenu(StatusMenu *menu) { m_statusMenu = menu; }
  static QIcon icon(const QIcon &icon, const QString &overlay);
  static QIcon icon(const QString &file, const QString &overlay);
  static QIcon icon(IconName name);
  void startNotify(int notice, const QVariant &data = QVariant());

signals:
  void channelDataChanged(const QByteArray &senderId, const QByteArray &channelId);
  void message(const AbstractMessage &message);
  void notify(int notice, const QVariant &data);

public slots:
  inline void openUrl(const QString &url) { openUrl(QUrl(url)); }
  void click(const QString &id, const QString &button);
  void openUrl(const QUrl &url);
  void send(const QString &text);

private slots:
  void settingsChanged(const QString &key, const QVariant &value);
  void start();

private:
  void loadTranslation();

  ChatCorePrivate *d;                             ///< Приватный класс.
  ChatPlugins *m_plugins;                         ///< Загрузчик плагинов.
  ChatSettings *m_settings;                       ///< Настройки.
  FileLocations *m_locations;                     ///< Схема размещения файлов.
  MessageAdapter *m_messageAdapter;               ///< Адаптер отправки и получения сообщений.
  NetworkManager *m_networkManager;               ///< Объект управляющих сетями.
  QByteArray m_currentId;                         ///< Идентификатор текущей вкладки.
  QMultiHash<QString, ChatViewAction*> m_actions; ///< Web действия.
  SimpleClient *m_client;                         ///< Клиент.
  static ChatCore *m_self;                        ///< Указатель на себя.
  StatusMenu *m_statusMenu;                       ///< Меню статуса.
  Translation *m_translation;                     ///< Модуль загрузки переводов.
};

#endif /* CHATCORE_H_ */
