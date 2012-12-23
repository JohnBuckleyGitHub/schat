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

#include "ChannelsCmd.h"
#include "ChannelsPlugin_p.h"
#include "client/ChatClient.h"
#include "client/ClientCmd.h"
#include "client/ClientMessages.h"
#include "sglobal.h"

ChannelsCmd::ChannelsCmd(QObject *parent)
  : Messages(parent)
{
  ChatClient::messages()->add(this);
}


bool ChannelsCmd::command(const QByteArray &dest, const ClientCmd &cmd)
{
  Q_UNUSED(dest)
  const QString command = cmd.command().toLower();

  if (command == LS("ignore"))
    ChannelsPluginImpl::ignore(dest);

  else if (command == LS("unignore"))
    ChannelsPluginImpl::unignore(dest);

  else if (command == LS("list"))
    ChannelsPluginImpl::show();

  else
    return false;

  return true;
}
