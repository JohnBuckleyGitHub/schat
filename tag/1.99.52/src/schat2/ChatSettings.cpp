/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include <QFile>

#include "Channel.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/ClientFeeds.h"
#include "client/SimpleClient.h"
#include "sglobal.h"
#include "ChatNotify.h"

ChatSettings::ChatSettings(const QString &fileName, const QString &defaultFile, QObject *parent)
  : Settings(fileName, parent)
  , m_synced(false)
  , m_settings(0)
{
  if (QFile::exists(defaultFile)) {
    m_settings = new QSettings(defaultFile, QSettings::IniFormat, this);
    m_settings->setIniCodec("UTF-8");
  }

  setLocalDefault(LS("AutoConnect"),           true);
  setLocalDefault(LS("AutoJoin"),              true);
  setLocalDefault(LS("DefaultServer"),         LS("schat://schat.me"));
  setLocalDefault(LS("Labs/CookieAuth"),       true);
  setLocalDefault(LS("Labs/DeveloperExtras"),  false);
  setLocalDefault(LS("Labs/DisableUI"),        false);
  setLocalDefault(LS("Maximized"),             false);
  setLocalDefault(LS("Networks"),              QStringList());
  setLocalDefault(LS("Profile/Gender"),        0);
  setLocalDefault(LS("Profile/Nick"),          Channel::defaultName());
  setLocalDefault(LS("Profile/Status"),        1);
  setLocalDefault(LS("WindowsAero"),           true);

  setDefault(LS("Display/Seconds"),            false);
  setDefault(LS("Display/Service"),            false);
  setDefault(LS("Labs/StaticTrayAlerts"),      false);
  setDefault(LS("Translation"),                LS("auto"));
  setDefault(LS("ToolBarActions"),             QStringList());
}


QVariant ChatSettings::setDefaultAndRead(const QString &key, const QVariant &value)
{
  setDefault(key, value);
  return this->value(key);
}


void ChatSettings::init()
{
  connect(ChatClient::i(), SIGNAL(ready()), SLOT(ready()));
  connect(ChatClient::i(), SIGNAL(offline()), SLOT(offline()));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));
}


void ChatSettings::setDefault(const QString &key, const QVariant &value)
{
  if (m_settings)
    Settings::setDefault(key, m_settings->value(key, value));
  else
    Settings::setDefault(key, value);
}


void ChatSettings::setLocalDefault(const QString &key, const QVariant &value)
{
  setDefault(key, value);

  if (!m_local.contains(key))
    m_local.append(key);
}


void ChatSettings::setValue(const QString &key, const QVariant &value, bool notify, bool local)
{
  if (QSettings::value(key, m_default.value(key)) == value)
    return;

  Settings::setValue(key, value, notify);
  if (local || m_local.contains(key))
    return;

  if (ChatClient::state() == ChatClient::Online)
    set(key, value);
}


void ChatSettings::notify(const Notify &notify)
{
  if (notify.type() == Notify::FeedData) {
    const FeedNotify &n = static_cast<const FeedNotify &>(notify);
    if (n.name() == LS("settings") && n.channel() == ChatClient::id())
      set(n.status() == Notice::Found);
  }
}


void ChatSettings::offline()
{
  m_synced = false;
}


void ChatSettings::ready()
{
  FeedPtr feed = ChatClient::channel()->feed(LS("settings"), false);

  if (!feed) {
    const QByteArray id = ChatClient::id();
    ChatClient::io()->lock();
    ClientFeeds::post(id, LS("settings"));
    ClientFeeds::put(id, LS("settings/head/mask"), 0700);
    ClientFeeds::request(id, LS("get"), LS("settings"));
    ChatClient::io()->unlock();
  }
  else if (!m_synced)
    ClientFeeds::request(ChatClient::channel(), LS("get"), LS("settings"));
}


/*!
 * Синхронизация настроек с сервером.
 *
 * \param offline \b true если используется кэшированная версия фида.
 */
void ChatSettings::set(bool offline)
{
  FeedPtr feed = ChatClient::channel()->feed(LS("settings"), false);
  if (!feed)
    return;

  QStringList keys = feed->data().keys();
  keys.removeAll(LS("head"));

  if (!m_synced) {
    QMapIterator<QString, QVariant> i(m_default);
    while (i.hasNext()) {
      i.next();
      if (!m_local.contains(i.key()))
        Settings::setValue(i.key(), keys.contains(i.key()) ? feed->data().value(i.key()) : i.value());
    }
  }
  else {
    foreach (const QString &key, keys) {
      if (!m_local.contains(key))
        Settings::setValue(key, feed->data().value(key));
    }
  }

  if (!offline && ChatClient::state() == ChatClient::Online && !m_synced) {
    m_synced = true;
    emit synced();
  }
}


void ChatSettings::set(const QString &key, const QVariant &value)
{
  if (key.isEmpty())
    return;

  ClientFeeds::post(ChatClient::id(), LS("settings/") + key, value, Feed::Echo | Feed::Share);
}