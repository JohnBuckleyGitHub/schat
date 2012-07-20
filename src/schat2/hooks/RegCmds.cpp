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
#include "client/ChatClient.h"
#include "client/ClientCmd.h"
#include "client/ClientFeeds.h"
#include "client/ClientMessages.h"
#include "client/SimpleClient.h"
#include "hooks/RegCmds.h"
#include "net/SimpleID.h"
#include "NetworkManager.h"
#include "sglobal.h"

RegCmds::RegCmds(QObject *parent)
  : Messages(parent)
{
  ChatClient::messages()->hooks()->add(this);
}


bool RegCmds::command(const QByteArray &dest, const ClientCmd &cmd)
{
  QString command = cmd.command().toLower();
  if (command == LS("login")) {
    ClientCmd body(cmd.body());
//    ChatClient::i()->login(body.command(), body.body());
    return true;
  }
  else if (command == LS("reg")) {
    ClientCmd body(cmd.body());
    reg(body.command(), body.body());
    return true;
  }
  else if (command == LS("sign")) {
    ClientCmd body(cmd.body());
    if (!body.isValid())
      return true;

    if (body.command() == LS("in")) {
      return this->command(dest, LS("login ") + body.body());
    }
    else if (body.command() == LS("up")) {
      return this->command(dest, LS("reg ") + body.body());
    }
    else if (body.command() == LS("out")) {
      signOut();
      return true;
    }

    return true;
  }

  return false;
}


void RegCmds::signOut()
{
  if (ChatClient::state() != ChatClient::Online)
    return;

  Network item = ChatCore::networks()->item(ChatClient::serverId());
  if (!item)
    return;

  ChatClient::feeds()->query(LS("hosts"), LS("unlink"));

  ChatClient::io()->leave();
  item->setCookie(QByteArray());
  ChatClient::open();
}


void RegCmds::reg(const QString &name, const QString &password)
{
  if (ChatClient::state() != ChatClient::Online)
    return;

  if (name.isEmpty())
    return;

  if (password.isEmpty())
    return;

  QVariantMap data;
  data[LS("name")] = name;
  data[LS("pass")] = SimpleID::encode(SimpleID::password(password));
  ChatClient::feeds()->query(LS("account"), LS("reg"), data);
}
