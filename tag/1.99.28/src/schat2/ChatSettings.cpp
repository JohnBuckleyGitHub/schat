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

#include "Channel.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/ClientFeeds.h"
#include "client/SimpleClient.h"
#include "sglobal.h"
#include "ChatNotify.h"

ChatSettings::ChatSettings(const QString &fileName, QObject *parent)
  : Settings(fileName, parent)
{
  setLocalDefault(LS("AutoConnect"),           true);
  setLocalDefault(LS("AutoJoin"),              true);
  setLocalDefault(LS("Height"),                420);
  setLocalDefault(LS("Labs/CookieAuth"),       true);
  setLocalDefault(LS("Labs/DeveloperExtras"),  false);
  setLocalDefault(LS("Labs/DisableUI"),        false);
  setLocalDefault(LS("Maximized"),             false);
  setLocalDefault(LS("Networks"),              QStringList());
  setLocalDefault(LS("Profile/Gender"),        0);
  setLocalDefault(LS("Profile/Nick"),          Channel::defaultName());
  setLocalDefault(LS("Profile/Status"),        1);
  setLocalDefault(LS("Width"),                 666);
  setLocalDefault(LS("WindowsAero"),           true);

  setDefault(LS("Display/Seconds"),            false);
  setDefault(LS("Display/Service"),            false);
  setDefault(LS("Labs/StaticTrayAlerts"),      false);
  setDefault(LS("Translation"),                LS("auto"));
}


void ChatSettings::init()
{
  connect(ChatClient::i(), SIGNAL(ready()), SLOT(ready()));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));
}


void ChatSettings::setLocalDefault(const QString &key, const QVariant &value)
{
  setDefault(key, value);

  if (!m_local.contains(key))
    m_local.append(key);
}


void ChatSettings::setValue(const QString &key, const QVariant &value, bool notify)
{
  if (QSettings::value(key, m_default.value(key)) == value)
    return;

  Settings::setValue(key, value, notify);
  if (m_local.contains(key))
    return;

  if (ChatClient::state() == ChatClient::Online)
    set(key, value);
}


void ChatSettings::notify(const Notify &notify)
{
  if (notify.type() == Notify::FeedData) {
    const FeedNotify &n = static_cast<const FeedNotify &>(notify);
    if (n.name() != LS("settings"))
      return;

    if (n.channel() != ChatClient::id())
      return;

    set();
  }
  else if (notify.type() == Notify::FeedReply) {
    const FeedNotify &n = static_cast<const FeedNotify &>(notify);
    if (n.match(ChatClient::id(), LS("settings"), LS("x-set")))
      set(n);
  }
}


void ChatSettings::ready()
{
  FeedPtr feed = ChatClient::channel()->feed(LS("settings"), false);
  if (!feed) {
    QVariantMap query;
    query[LS("action")] = LS("x-mask");
    query[LS("mask")] = 0700;

    ChatClient::io()->lock();
    ChatClient::feeds()->request(ChatClient::id(), LS("add"), LS("settings"));
    ChatClient::feeds()->request(ChatClient::id(), LS("query"), LS("settings"), query);
    ChatClient::io()->unlock();
  }
}


void ChatSettings::set()
{
  FeedPtr feed = ChatClient::channel()->feed(LS("settings"), false);
  if (!feed)
    return;

  QStringList keys = feed->data().keys();
  keys.removeAll(LS("head"));
  if (keys.isEmpty())
    return;

  foreach (QString key, keys) {
    if (!m_local.contains(key))
      Settings::setValue(key, feed->data().value(key));
  }
}


void ChatSettings::set(const FeedNotify &notify)
{
  QStringList keys = notify.json().keys();
  keys.removeAll(LS("action"));
  if (keys.isEmpty())
    return;

  foreach (QString key, keys) {
    if (!m_local.contains(key))
      Settings::setValue(key, notify.json().value(key));
  }
}


void ChatSettings::set(const QString &key, const QVariant &value)
{
  QVariantMap query;
  query[key] = value;
  ChatClient::feeds()->query(LS("settings"), LS("x-set"), query);
}
