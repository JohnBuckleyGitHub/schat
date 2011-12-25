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

#include "client/ChatClient.h"
#include "client/ClientCmd.h"
#include "client/ClientFeeds.h"
#include "client/ClientMessages.h"
#include "RawFeedsCmd.h"
#include "RawFeedsPlugin_p.h"

namespace Hooks
{

RawFeedsCmd::RawFeedsCmd(RawFeeds *parent)
  : Messages(parent)
{
  ChatClient::messages()->hooks()->add(this);
}


bool RawFeedsCmd::command(const QByteArray &dest, const ClientCmd &cmd)
{
  qDebug() << "RawFeedsCmd" << cmd.command();
  QString command = cmd.command().toLower();
  if (command == "feeds") {
    ChatClient::feeds()->headers(dest);
    return true;
  }

  return false;
}

} // namespace Hooks
