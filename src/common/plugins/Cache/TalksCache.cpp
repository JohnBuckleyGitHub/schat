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
#include "ChatSettings.h"
#include "sglobal.h"
#include "TalksCache.h"
#include "ChatNotify.h"
#include "net/SimpleID.h"

TalksCache::TalksCache(QObject *parent)
  : QObject(parent)
  , m_settings(ChatCore::settings())
{
  m_settings->setDefault(LS("RecentTalks"),    QStringList());
  m_settings->setDefault(LS("MaxRecentTalks"), 20);

  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));
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
