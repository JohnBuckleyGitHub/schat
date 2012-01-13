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
#include "hooks/UserMenuImpl.h"
#include "net/SimpleID.h"
#include "ui/ChatIcons.h"

namespace Hooks
{

UserMenuImpl::UserMenuImpl(QObject *parent)
  : ChannelMenu(parent)
  , m_insert(0)
  , m_talk(0)
{
  add(this);
}

void UserMenuImpl::bindImpl(QMenu *menu, ClientChannel channel)
{
  if (channel->type() != SimpleID::UserId)
    return;

  if (ChatCore::currentId() != channel->id()) {
    m_talk = new QAction(SCHAT_ICON(Balloon), Hooks::UserMenuImpl::tr("Talk..."), this);
    m_talk->setData(ChatUrls::toUrl(channel, "open"));
    menu->addAction(m_talk);
  }

  m_insert = new QAction(Hooks::UserMenuImpl::tr("Insert Nick"), this);
  m_insert->setData(ChatUrls::toUrl(channel, "insert"));
  menu->addAction(m_insert);
}


void UserMenuImpl::cleanupImpl()
{
  if (m_insert) delete m_insert; m_insert = 0;
  if (m_talk)   delete m_insert; m_insert = 0;
}

} // namespace Hooks
