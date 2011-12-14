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
class MessageData;
class NetworkManager;
class SimpleClient;
class Translation;

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
    TopicEdit,
    KeyIcon,
    ExclamationRedIcon,
    ArrowRightIcon
  };

  ChatCore(QObject *parent = 0);
  ~ChatCore();
  bool isIgnored(const QByteArray &id);
  inline ChatPlugins *plugins() { return m_plugins; }
  inline const QByteArray& currentId() const { return m_currentId; }
  inline static ChatCore *i()              { return m_self; }
  inline static ChatSettings *settings()   { return m_self->m_settings; }
  inline static FileLocations *locations() { return m_self->m_locations; }
  inline static NetworkManager *networks() { return m_self->m_networkManager; }
  inline Translation *translation() { return m_translation; }
  inline void addChatViewAction(const QString &id, ChatViewAction *action) { m_actions.insert(id, action); }
  inline void setCurrentId(const QByteArray &id) { m_currentId = id; }
  static QIcon icon(IconName name);
  static void makeRed(QWidget *widget, bool red = true);

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
  NetworkManager *m_networkManager;               ///< Объект управляющих сетями.
  QByteArray m_currentId;                         ///< Идентификатор текущей вкладки.
  QMultiHash<QString, ChatViewAction*> m_actions; ///< Web действия.
  SimpleClient *m_client;                         ///< Клиент.
  static ChatCore *m_self;                        ///< Указатель на себя.
  Translation *m_translation;                     ///< Модуль загрузки переводов.
};

#endif /* CHATCORE_H_ */
