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

#include <QPainter>
#include <QTextDocument>
#include <QTimer>

#include "ChatCore.h"
#include "ChatSettings.h"
#include "FileLocations.h"
#include "messages/MessageAdapter.h"
#include "net/packets/message.h"
#include "net/SimpleClient.h"
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

  m_settings = new ChatSettings(this);
  m_settings->read();

  loadTranslation();

  m_client = new SimpleClient(new User("IMPOMEZIA"), 0, this);
  m_settings->setClient(m_client);

  m_messageAdapter = new MessageAdapter(this);
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

  connect(m_messageAdapter, SIGNAL(message(const AbstractMessage &)), SIGNAL(message(const AbstractMessage &)));
  connect(m_settings, SIGNAL(changed(const QList<int> &)), SLOT(settingsChanged(const QList<int> &)));

  QTimer::singleShot(0, this, SLOT(start()));
}


ChatCore::~ChatCore()
{
  delete m_userUtils;
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


QIcon ChatCore::icon(IconName name)
{
  if (name < m_icons.size())
    return QIcon(":/images/" + m_icons.at(name) + ".png");

  return QIcon();
}


/*!
 * Отправка сообщения.
 */
void ChatCore::send(const QByteArray &destId, const QString &text)
{
  if (text.isEmpty())
    return;

  MessageData data(QByteArray(), destId, text);
  m_messageAdapter->send(data);
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

  ChatUser user = m_client->user(id);
  if (!user)
    return;

  startNotify(InsertTextToSend, " <b>" + Qt::escape(user->nick()) + "</b> ");
}


void ChatCore::settingsChanged(const QList<int> &keys)
{
  if (keys.contains(ChatSettings::Translation)) {
    m_translation->load(m_settings->value(ChatSettings::Translation).toString());
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
  m_translation->setSearch(QStringList() << (m_settings->locations()->path(FileLocations::SharePath) + "/translations") << (m_settings->locations()->path(FileLocations::ConfigPath) + "/translations"));
  m_translation->load(m_settings->value(ChatSettings::Translation).toString());
}
