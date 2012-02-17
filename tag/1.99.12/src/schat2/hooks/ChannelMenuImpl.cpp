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

#include "ChatCore.h"
#include "ChatUrls.h"
#include "hooks/ChannelMenuImpl.h"
#include "net/SimpleID.h"
#include "ui/ChatIcons.h"

namespace Hooks
{

ChannelMenuImpl::ChannelMenuImpl(QObject *parent)
  : ChannelMenu(parent)
  , m_topic(0)
{
  add(this);
}


void ChannelMenuImpl::bindImpl(QMenu *menu, ClientChannel channel)
{
  if (channel->type() != SimpleID::ChannelId)
    return;

  m_topic = new QAction(SCHAT_ICON(TopicEdit), Hooks::ChannelMenuImpl::tr("Edit topic..."), this);
  m_topic->setData(ChatUrls::toUrl(channel, "edit/topic"));
  menu->addAction(m_topic);
}


void ChannelMenuImpl::cleanupImpl()
{
  if (m_topic)
    delete m_topic;

  m_topic = 0;
}

} // namespace Hooks
