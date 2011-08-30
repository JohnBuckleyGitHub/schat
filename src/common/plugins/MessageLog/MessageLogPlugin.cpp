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

#include <QDebug>

#include <qplugin.h>

#include "MessageLogPlugin.h"
#include "MessageLogPlugin_p.h"

MessageLog::MessageLog(Core *core)
  : NodePlugin(core)
{
}


HookResult MessageLog::hook(const NodeHook &data)
{
  qDebug() << "------------------------";
  qDebug() << "HOOK";
  qDebug() << "------------------------";
  return HookResult(1);
}


QList<NodeHook::Type> MessageLog::hooks() const
{
  QList<NodeHook::Type> out;
  out += NodeHook::AcceptedMessage;

  return out;
}


NodePlugin *MessageLogPlugin::init(Core *core)
{
  m_plugin = new MessageLog(core);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(MessageLog, MessageLogPlugin);
