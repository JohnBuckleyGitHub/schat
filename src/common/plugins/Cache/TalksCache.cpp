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

#include "ChatCore.h"
#include "ChatNotify.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "TalksCache.h"

TalksCache::TalksCache(QObject *parent)
  : QObject(parent)
  , m_settings(ChatCore::settings())
{
  m_settings->setDefault(LS("RecentTalks"),    QStringList());
  m_settings->setDefault(LS("MaxRecentTalks"), 20);

  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));
  connect(m_settings, SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));
  connect(m_settings, SIGNAL(synced()), SLOT(synced()));
}


void TalksCache::notify(const Notify &notify)
{
  if (notify.type() == Notify::ChannelTabClosed) {
    QString id         = SimpleID::encode(notify.data().toByteArray());
    QStringList recent = m_settings->value(LS("RecentTalks")).toStringList().mid(0, m_settings->value(LS("MaxRecentTalks")).toInt());
    recent.removeAll(id);
    recent.prepend(id);

    m_settings->setValue(LS("RecentTalks"), recent);
  }
}


void TalksCache::settingsChanged(const QString &key, const QVariant &value)
{
  Q_UNUSED(value)

  if (key == LS("RecentTalks") || key == LS("MaxRecentTalks"))
    m_channels = channels();
}


void TalksCache::synced()
{
  m_channels = channels();
  ClientChannels *channels = ChatClient::channels();
  QList<QByteArray> unsynced;

  foreach (const QByteArray &id, m_channels) {
    ClientChannel channel = channels->get(id);
    if (!channel || !channel->isSynced())
      unsynced.append(id);
  }

  channels->info(unsynced);
}


/*!
 * Получение списка каналов.
 */
QList<QByteArray> TalksCache::channels() const
{
  QStringList recents = m_settings->value(LS("RecentTalks")).toStringList().mid(0, m_settings->value(LS("MaxRecentTalks")).toInt());
  int type = 0;
  QList<QByteArray> list;

  foreach (const QString &text, recents) {
    QByteArray id = SimpleID::decode(text);
    type = SimpleID::typeOf(id);

    if (type == SimpleID::ChannelId || type == SimpleID::UserId)
      list.append(id);
  }

  return list;
}
