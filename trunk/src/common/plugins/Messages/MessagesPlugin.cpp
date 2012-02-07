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

#include <QtPlugin>

#include "cores/Core.h"
#include "MessagesPlugin.h"
#include "MessagesPlugin_p.h"
#include "NodeMessages.h"
#include "NodeMessagesDB.h"

NodeMessagesBase::NodeMessagesBase(QObject *parent)
  : NodePlugin(parent)
{
  new NodeMessages(Core::i());

  NodeMessagesDB::open();
}


NodePlugin *MessagesPlugin::create()
{
  m_plugin = new NodeMessagesBase(this);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(Messages, MessagesPlugin);
