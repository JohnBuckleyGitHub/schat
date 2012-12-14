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

#include "client/ChatClient.h"
#include "client/ClientMessages.h"
#include "ConsoleCmd.h"
#include "ConsolePlugin_p.h"
#include "sglobal.h"
#include "client/ClientCmd.h"

ConsoleCmd::ConsoleCmd(ConsolePluginImpl *plugin)
  : Messages(plugin)
  , m_plugin(plugin)
{
  ChatClient::messages()->add(this);
}


bool ConsoleCmd::command(const QByteArray &dest, const ClientCmd &cmd)
{
  Q_UNUSED(dest)

  QString command = cmd.command().toLower();
  if (command == LS("console")) {
    m_plugin->show();
    return true;
  }

  return false;
}
