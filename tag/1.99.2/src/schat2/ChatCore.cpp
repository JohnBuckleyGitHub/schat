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
#include "ChatAlerts.h"
#include "ChatCore.h"
#include "ChatHooks.h"
#include "ChatNotify.h"
#include "ChatPlugins.h"
#include "ChatSettings.h"
#include "ChatUrls.h"
#include "client/ChatClient.h"
#include "client/ClientMessages.h"
#include "client/SimpleClient.h"
#include "FileLocations.h"
#include "hooks/ChannelMenu.h"
#include "hooks/ChannelMenuImpl.h"
#include "hooks/ChannelsImpl.h"
#include "hooks/ChatViewHooks.h"
#include "hooks/ClientImpl.h"
#include "hooks/CommandsImpl.h"
#include "hooks/MessagesImpl.h"
#include "hooks/UserMenuImpl.h"
#include "net/SimpleID.h"
#include "NetworkManager.h"
#include "text/HtmlFilter.h"
#include "text/PlainTextFilter.h"
#include "Translation.h"
#include "ui/ChatIcons.h"

ChatCore *ChatCore::m_self = 0;



ChatCore::ChatCore(QObject *parent)
  : QObject(parent)
{
  m_self = this;

  qsrand(QDateTime::currentDateTime().toTime_t());

  new ChatUrls(this);

  m_locations = new FileLocations(this);
  m_settings = new ChatSettings(m_locations->path(FileLocations::ConfigFile), this);

  loadTranslation();

  new ChatClient(this);
  new ChatNotify(this);

  new Hooks::MessagesImpl(this);
  new Hooks::CommandsImpl(this);
  new Hooks::ChannelsImpl(this);
  new Hooks::ClientImpl(this);

  new Hooks::ChannelMenu(this);
//  new Hooks::ChannelMenuImpl(this);
  new Hooks::UserMenuImpl(this);

  new ChatViewHooks(this);

  new ChatAlerts(this);

  m_client = ChatClient::io();

  m_networkManager = new NetworkManager(this);
  ChatClient::id(); // Необходимо для инициализации базовых настроек.

  ChatIcons::init();

  m_plugins = new ChatPlugins(this);
  m_plugins->load();

  connect(m_settings, SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));

  QTimer::singleShot(0, this, SLOT(start()));
}


ChatCore::~ChatCore()
{
  qDeleteAll(m_actions);
}


QByteArray ChatCore::randomId()
{
  return SimpleID::randomId(SimpleID::MessageId, ChatClient::id());
}


/*!
 * \deprecated Эта функция не должна находится в этом классе.
 */
void ChatCore::makeRed(QWidget *widget, bool red)
{
  QPalette palette = widget->palette();

  if (red)
    palette.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));
  else
    palette.setColor(QPalette::Active, QPalette::Base, Qt::white);

  widget->setPalette(palette);
}


/*!
 * \deprecated Необходим отдельный класс.
 */
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
 * Отправка сообщения.
 */
void ChatCore::send(const QString &text)
{
  if (text.isEmpty())
    return;

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
