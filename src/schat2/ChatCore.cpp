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
#include <QDesktopServices>
#include <QPainter>
#include <QTextDocument>
#include <QTimer>
#include <QWidget>

#include "actions/ChatViewAction.h"
#include "ChatCore.h"
#include "ChatCore_p.h"
#include "ChatHooks.h"
#include "ChatNotify.h"
#include "ChatPlugins.h"
#include "ChatSettings.h"
#include "ChatUrls.h"
#include "client/ChatClient.h"
#include "client/ClientMessages.h"
#include "client/SimpleClient.h"
#include "FileLocations.h"
#include "hooks/ChannelsImpl.h"
#include "hooks/ClientImpl.h"
#include "hooks/MessagesImpl.h"
#include "messages/MessageAdapter.h"
#include "net/packets/messages.h"
#include "NetworkManager.h"
#include "text/HtmlFilter.h"
#include "text/PlainTextFilter.h"
#include "Translation.h"
#include "User.h"

ChatCore *ChatCore::m_self = 0;
QStringList ChatCorePrivate::icons;


ChatCorePrivate::ChatCorePrivate()
{
  icons += QLatin1String("channel");
  icons += QLatin1String("channel-alert");
  icons += QLatin1String("gear");
  icons += QLatin1String("main-tab-menu");
  icons += QLatin1String("network-error");
  icons += QLatin1String("offline");
  icons += QLatin1String("online");
  icons += QLatin1String("plug");
  icons += QLatin1String("plug-disconnect");
  icons += QLatin1String("quit");
  icons += QLatin1String("secure");
  icons += QLatin1String("settings");
  icons += QLatin1String("sound");
  icons += QLatin1String("sound_mute");
  icons += QLatin1String("users");
  icons += QLatin1String("schat16");
  icons += QLatin1String("schat16-ny");
  icons += QLatin1String("text-bold");
  icons += QLatin1String("text-italic");
  icons += QLatin1String("text-strikethrough");
  icons += QLatin1String("text-underline");
  icons += QLatin1String("send");
  icons += QLatin1String("globe");
  icons += QLatin1String("add");
  icons += QLatin1String("remove");
  icons += QLatin1String("profile");
  icons += QLatin1String("ok");
  icons += QLatin1String("information-balloon");
  icons += QLatin1String("edit-clear");
  icons += QLatin1String("edit-copy");
  icons += QLatin1String("edit-cut");
  icons += QLatin1String("edit-paste");
  icons += QLatin1String("edit-select-all");
  icons += QLatin1String("balloon");
  icons += QLatin1String("slash");
  icons += QLatin1String("topic-edit");
  icons += QLatin1String("key");
  icons += QLatin1String("exclamation-red");
  icons += QLatin1String("arrow-right");
}


ChatCorePrivate::~ChatCorePrivate()
{
}


void ChatCorePrivate::ignore(const QByteArray &id)
{
  if (SimpleID::typeOf(id) != SimpleID::UserId)
    return;

  if (!ignoreList.contains(id))
    ignoreList.prepend(id);

  writeIgnoreList();
}


void ChatCorePrivate::loadIgnoreList()
{
  ignoreList.clear();
//  QStringList list = q->settings()->value(SimpleID::encode(q->networks()->serverId()) + QLatin1String("/IgnoreList"), QStringList()).toStringList();
}


/*!
 * Открытия адресов вида chat://channel/идентификатор канала/действие.
 *
 * \deprecated Вместо этой функции использовать ChatUrls.
 */
void ChatCorePrivate::openChannelUrl(const QUrl &url)
{
  QStringList actions = ChatUrls::actions(url);
  if (actions.isEmpty())
    return;

  ClientChannel channel = ChatUrls::channel(url);
  if (!channel)
    return;

  if (actions.first() == "edit") {
    if (actions.size() == 1)
      return;

    if (actions.at(1) == "topic")
      q->startNotify(ChatCore::EditTopicNotice, channel->id());
  }
}


/*!
 * Открытия адресов вида chat://user/идентификатор пользователя/действие.
 *
 * \deprecated Вместо этой функции использовать ChatUrls.
 */
void ChatCorePrivate::openUserUrl(const QUrl &url)
{
  QStringList actions = ChatUrls::actions(url);
  if (actions.isEmpty())
    return;

  ClientUser user;
  if (!user)
    return;

  if (actions.first() == QLatin1String("ignore")) {
    ignore(user->id());
  }
  else if (actions.first() == QLatin1String("unignore")) {
    unignore(user->id());
  }
}


void ChatCorePrivate::unignore(const QByteArray &id)
{
  if (SimpleID::typeOf(id) != SimpleID::UserId)
    return;

  ignoreList.removeAll(id);

  writeIgnoreList();
}


void ChatCorePrivate::writeIgnoreList()
{
  QStringList list;
  for (int i = 0; i < ignoreList.size(); ++i) {
    list.append(SimpleID::encode(ignoreList.at(i)));
  }

//  q->settings()->setValue(SimpleID::encode(q->networks()->serverId()) + QLatin1String("/IgnoreList"), list);
}


ChatCore::ChatCore(QObject *parent)
  : QObject(parent)
  , d(new ChatCorePrivate())
{
  m_self = this;
  d->q = this;

  qsrand(QDateTime::currentDateTime().toTime_t());

  new ChatUrls(this);

  m_locations = new FileLocations(this);
  m_settings = new ChatSettings(m_locations->path(FileLocations::ConfigFile), this);

  loadTranslation();

  new ChatClient(this);
  new ChatNotify(this);

  new Hooks::MessagesImpl(this);
  new Hooks::ChannelsImpl(this);
  new Hooks::ClientImpl(this);

  m_client = ChatClient::io();

  m_networkManager = new NetworkManager(this);
  ChatClient::id(); // Необходимо для инициализации базовых настроек.

  m_plugins = new ChatPlugins(this);
  m_plugins->load();

  connect(m_settings, SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));

  QTimer::singleShot(0, this, SLOT(start()));
}


ChatCore::~ChatCore()
{
  delete d;
  qDeleteAll(m_actions);
}


bool ChatCore::isIgnored(const QByteArray &id)
{
  return d->ignoreList.contains(id);
}


/*!
 * \todo Добавить поддержку получения иконки из темы.
 */
QIcon ChatCore::icon(IconName name)
{
  if (name >= ChatCorePrivate::icons.size())
    return QIcon();

  return QIcon(QLatin1String(":/images/") + ChatCorePrivate::icons.at(name) + QLatin1String(".png"));
}


void ChatCore::makeRed(QWidget *widget, bool red)
{
  QPalette palette = widget->palette();

  if (red)
    palette.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));
  else
    palette.setColor(QPalette::Active, QPalette::Base, Qt::white);

  widget->setPalette(palette);
}


void ChatCore::startNotify(int notice, const QVariant &data)
{
  emit notify(notice, data);
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
 * Обработчик ссылок.
 */
void ChatCore::openUrl(const QUrl &url)
{
  qDebug() << "-----" << url;

  if (url.scheme() == QLatin1String("schat")) {
    m_client->openUrl(url);
    return;
  }

  if (url.scheme() != QLatin1String("chat")) {
    QDesktopServices::openUrl(url);
    return;
  }

  if (url.host() == QLatin1String("user")) {
    d->openUserUrl(url);
  }
  else if (url.host() == QLatin1String("channel")) {
    d->openChannelUrl(url);
  }
  else if (url.host() == QLatin1String("about")) {
    startNotify(ChatCore::AboutNotice);
  }
  else if (url.host() == QLatin1String("settings")) {
    startNotify(ChatCore::SettingsNotice, url);
  }
}


/*!
 * Отправка сообщения.
 */
void ChatCore::send(const QString &text)
{
  if (text.isEmpty())
    return;

  MessageData data(QByteArray(), m_currentId, QString(), text);
  ChatClient::messages()->send(m_currentId, text);
}


void ChatCore::settingsChanged(const QString &key, const QVariant &value)
{
  if (key == QLatin1String("Translation")) {
    m_translation->load(value.toString());
  }
}


void ChatCore::start()
{
  ChatClient::open();
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
