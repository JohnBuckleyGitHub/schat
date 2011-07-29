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

#include <QDateTime>
#include <QPainter>
#include <QTextDocument>
#include <QTimer>

#include "actions/ChatViewAction.h"
#include "ChatCore.h"
#include "ChatSettings.h"
#include "client/SimpleClient.h"
#include "FileLocations.h"
#include "messages/MessageAdapter.h"
#include "net/packets/message.h"
#include "NetworkManager.h"
#include "Plugins.h"
#include "Translation.h"
#include "ui/UserUtils.h"
#include "User.h"

ChatCore *ChatCore::m_self = 0;
QStringList ChatCore::m_icons;

ChatCore::ChatCore(QObject *parent)
  : QObject(parent)
  , m_statusMenu(0)
{
  m_self = this;
  qsrand(QDateTime::currentDateTime().toTime_t());

  m_userUtils = new UserUtils();

  m_locations = new FileLocations(this);
  m_settings = new ChatSettings(m_locations->path(FileLocations::ConfigFile), this);

  loadTranslation();

  m_client = new SimpleClient(this);
  m_settings->setClient(m_client);

  m_messageAdapter = new MessageAdapter();
  m_networkManager = new NetworkManager(this);

  m_plugins = new Plugins(this);
  m_plugins->load();

  m_icons += "channel";
  m_icons += "channel-alert";
  m_icons += "gear";
  m_icons += "main-tab-menu";
  m_icons += "network-error";
  m_icons += "offline";
  m_icons += "online";
  m_icons += "plug";
  m_icons += "plug-disconnect";
  m_icons += "quit";
  m_icons += "secure";
  m_icons += "settings";
  m_icons += "sound";
  m_icons += "sound_mute";
  m_icons += "users";
  m_icons += "schat16";
  m_icons += "schat16-ny";
  m_icons += "text-bold";
  m_icons += "text-italic";
  m_icons += "text-strikethrough";
  m_icons += "text-underline";
  m_icons += "send";
  m_icons += "globe";
  m_icons += "add";
  m_icons += "remove";
  m_icons += "profile";
  m_icons += "ok";
  m_icons += "information-balloon";
  m_icons += "edit-clear";
  m_icons += "edit-copy";
  m_icons += "edit-cut";
  m_icons += "edit-paste";
  m_icons += "edit-select-all";
  m_icons += "balloon";

  connect(m_messageAdapter, SIGNAL(message(const AbstractMessage &)), SIGNAL(message(const AbstractMessage &)));
  connect(m_settings, SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));

  QTimer::singleShot(0, this, SLOT(start()));
}


ChatCore::~ChatCore()
{
  qDeleteAll(m_actions);
  delete m_userUtils;
}


QIcon ChatCore::icon(const QIcon &icon, const QString &overlay)
{
  if (overlay.isEmpty())
    return icon;

  QPixmap pixmap = icon.pixmap(16, 16);
  QPainter painter(&pixmap);
  painter.drawPixmap(6, 6, QPixmap(overlay));
  painter.end();

  return QIcon(pixmap);
}


/*!
 * Наложение маленькой иконки \p overlay на большую \p file в правый нижний угол.
 *
 * \param file    Файл иконки размером 16x16 пикселей.
 * \param overlay Иконка-оверлей размером 10x10 пикселей.
 */
QIcon ChatCore::icon(const QString &file, const QString &overlay)
{
  if (overlay.isEmpty())
    return QIcon(file);

  QPixmap pixmap(file);
  QPainter painter(&pixmap);
  painter.drawPixmap(6, 6, QPixmap(overlay));
  painter.end();

  return QIcon(pixmap);
}


/*!
 * \todo Добавить поддержку получения иконки из темы.
 */
QIcon ChatCore::icon(IconName name)
{
  if (name >= m_icons.size())
    return QIcon();

  return QIcon(QLatin1String(":/images/") + m_icons.at(name) + QLatin1String(".png"));
}


void ChatCore::click(const QString &id, const QString &button)
{
  QList<ChatViewAction *> actions = m_actions.values(id);
  if (actions.isEmpty())
    return;

  ChatViewAction *action = 0;
  for (int i = 0; i < actions.size(); ++i) {
    action = actions.at(i);
    if (action && action->exec(id, button)) {
      m_actions.remove(id, action);
      delete action;
      action = 0;
    }
  }
}


/*!
 * Обработка одиночного клика по нику.
 * \todo ! Реализовать обработку двойного клика для открытия привата.
 */
void ChatCore::nickClicked(const QString &text)
{
  QByteArray id = userIdFromClass(text);
  if (id.isEmpty())
    return;

  ClientUser user = m_client->user(id);
  if (!user)
    return;

  startNotify(InsertTextToSend, " <b>" + Qt::escape(user->nick()) + "</b> ");
}


/*!
 * Отправка сообщения.
 */
void ChatCore::send(const QString &text)
{
  if (text.isEmpty())
    return;

  MessageData data(QByteArray(), m_currentId, text);
  m_messageAdapter->send(data);
}


void ChatCore::settingsChanged(const QString &key, const QVariant &value)
{
  if (key == QLatin1String("Translation")) {
    m_translation->load(value.toString());
  }
}


void ChatCore::start()
{
  m_networkManager->open();
}


/*!
 * Получение идентификатора пользователя из CSS класса.
 */
QByteArray ChatCore::userIdFromClass(const QString &text)
{
  if (text.size() < 47 || !text.startsWith("nick "))
    return QByteArray();

  return QByteArray::fromHex(text.mid(5, 42).toLatin1());
}


/*!
 * Загрузка перевода пользовательского интерфейса.
 */
void ChatCore::loadTranslation()
{
  m_translation = new Translation(this);
  m_translation->setSearch(QStringList() << (m_locations->path(FileLocations::SharePath) + QLatin1String("/translations")) << (m_locations->path(FileLocations::ConfigPath) + QLatin1String("/translations")));
  m_translation->load(m_settings->value(QLatin1String("Translation")).toString());
}
