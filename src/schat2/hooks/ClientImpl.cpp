/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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
#include "feeds/ServerFeed.h"
#include "hooks/ClientImpl.h"
#include "sglobal.h"

namespace Hooks
{

ClientImpl::ClientImpl(QObject *parent)
  : Client(parent)
{
  ChatClient::i()->hooks()->add(this);
}


/*!
 * \deprecated Вход в главный канал по имени а не по идентификатору, является устаревшим и нужен для совместимости с серверами версии ниже 1.99.56.
 */
void ClientImpl::setup()
{
  const int policy = ChatClient::channels()->policy();

  if (policy & ServerFeed::MainChannelPolicy && policy & ServerFeed::ForcedJoinPolicy) {
    const QByteArray id = ChatClient::channels()->mainId();
    if (id.isEmpty()) {
      const QStringList path = ChatUrls::path(ChatClient::io()->url());
      ChatClient::channels()->join(path.isEmpty() ? LS("Main") : path.at(0));
    }
    else
      ChatClient::channels()->join(id);
  }
}

} // namespace Hooks
