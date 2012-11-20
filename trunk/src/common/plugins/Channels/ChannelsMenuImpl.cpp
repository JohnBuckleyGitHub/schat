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

#include <QMenu>

#include "ChannelsMenuImpl.h"
#include "net/SimpleID.h"

ChannelsMenuImpl::ChannelsMenuImpl(QObject *parent)
  : ChannelMenu(parent)
  , m_ignore(0)
{
  add(this);
}


void ChannelsMenuImpl::bindImpl(QMenu *menu, ClientChannel channel, Hooks::Scope scope)
{
  if (channel->type() != SimpleID::UserId)
    return;

  menu->addSeparator();
  m_ignore = new QAction(tr("Ignore"), this);
  m_ignore->setCheckable(true);
  menu->addAction(m_ignore);
}


void ChannelsMenuImpl::cleanupImpl()
{
  if (m_ignore) delete m_ignore; m_ignore = 0;
}
