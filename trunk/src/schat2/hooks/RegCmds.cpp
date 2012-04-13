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
    ChatClient::i()->login(body.command(), body.body());
  }
  else if (command == LS("reg")) {
    ClientCmd body(cmd.body());
    if (!body.isValid())
      return true;

    ChatClient::feeds()->query(LS("account"), command, request(command, body.command(), body.body()));
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


QVariantMap RegCmds::request(const QString &action, const QString &name, const QString &password)
{
  if (name.isEmpty())
    return QVariantMap();

  if (password.isEmpty())
    return QVariantMap();

  QVariantMap out;
  out[LS("action")] = action;
  out[LS("name")]   = name.simplified().toLower().remove(LC(' '));
  out[LS("pass")]   = SimpleID::encode(SimpleID::password(password));

  return out;
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
