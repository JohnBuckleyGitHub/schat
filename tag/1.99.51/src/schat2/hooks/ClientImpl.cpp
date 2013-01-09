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
#include "ChatUrls.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/SimpleClient.h"
#include "hooks/ClientImpl.h"
#include "sglobal.h"

namespace Hooks
{

ClientImpl::ClientImpl(QObject *parent)
  : Client(parent)
{
  ChatClient::i()->hooks()->add(this);
}


void ClientImpl::setup()
{
  if (ChatCore::settings()->value(LS("AutoJoin")).toBool()) {
    const QStringList path = ChatUrls::path(ChatClient::io()->url());
    ChatClient::channels()->join(path.isEmpty() ? LS("Main") : path.at(0));
  }
}

} // namespace Hooks
