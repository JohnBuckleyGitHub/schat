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

#include <QMenu>

#include "actions/ChannelMenu.h"
#include "ChatCore.h"
#include "ui/ChannelUtils.h"

ChannelMenu::ChannelMenu(ClientChannel channel, QObject *parent)
  : MenuBuilder(parent)
  , m_channel(channel)
{
}


ChannelMenu *ChannelMenu::bind(QMenu *menu, const QVariant &id)
{
  ClientChannel channel = ChannelUtils::channel(id.toByteArray());
  if (!channel)
    return 0;

  ChannelMenu *out = new ChannelMenu(channel, menu);
  out->bind(menu);

  return out;
}


void ChannelMenu::bind(QMenu *menu)
{
  MenuBuilder::bind(menu);

  m_topic = new QAction(SCHAT_ICON(TopicEdit), tr("Edit topic..."), this);
  m_topic->setData(ChannelUtils::toUrl(m_channel, QLatin1String("edit/topic")));
  menu->addAction(m_topic);
}
