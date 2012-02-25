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

#include <QDebug>

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
  setDefault(LS("AutoConnect"),           true);
  setDefault(LS("AutoJoin"),              true);
  setDefault(LS("ChannelUserCount"),      false);
  setDefault(LS("DeveloperExtras"),       false);
  setDefault(LS("Height"),                420);
  setDefault(LS("HideIgnore"),            true);
  setDefault(LS("Maximized"),             false);
  setDefault(LS("Networks"),              QStringList());
  setDefault(LS("ShowSeconds"),           false);
  setDefault(LS("ShowServiceMessages"),   false);
  setDefault(LS("Translation"),           LS("auto"));
  setDefault(LS("Width"),                 666);
  setDefault(LS("WindowsAero"),           true);

  setDefault(LS("Labs/CookieAuth"),       true);
  setDefault(LS("Labs/DisableUI"),        false);
  setDefault(LS("Labs/StaticTrayAlerts"), false);

  setDefault(LS("Profile/Gender"),        0);
  setDefault(LS("Profile/Nick"),          Channel::defaultName());
  setDefault(LS("Profile/Status"),        1);
}


void ChatSettings::init()
{
  connect(ChatClient::i(), SIGNAL(ready()), SLOT(ready()));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));
}


void ChatSettings::notify(const Notify &notify)
{
  if (notify.type() == Notify::FeedData) {
    const FeedNotify &n = static_cast<const FeedNotify &>(notify);
    if (n.name() != LS("settings"))
      return;

    if (n.channel() != ChatClient::id())
      return;

    qDebug() << "DATA";
  }
}


void ChatSettings::ready()
{
  qDebug() << "";
  qDebug() << "";
  qDebug() << "ChatSettings::ready()";
  qDebug() << "";
  qDebug() << "";
  FeedPtr feed = ChatClient::channel()->feed(LS("settings"), false);
  if (!feed) {
    qDebug() << "FEED NOT EXIST";
    QVariantMap query;
    query[LS("action")] = LS("x-mask");
    query[LS("mask")] = 0700;

    ChatClient::io()->lock();
    ChatClient::feeds()->request(ChatClient::id(), LS("add"), LS("settings"));
    ChatClient::feeds()->request(ChatClient::id(), LS("query"), LS("settings"), query);
    ChatClient::io()->unlock();
  }
}
