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

#include <QDebug>

#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientCmd.h"
#include "client/ClientFeeds.h"
#include "client/ClientMessages.h"
#include "JSON.h"
#include "RawFeedsCmd.h"
#include "RawFeedsMessage.h"
#include "RawFeedsPlugin_p.h"
#include "sglobal.h"
#include "ui/TabWidget.h"

namespace Hooks
{

RawFeedsCmd::RawFeedsCmd(RawFeeds *parent)
  : Messages(parent)
{
  ChatClient::messages()->hooks()->add(this);
}


bool RawFeedsCmd::command(const QByteArray &dest, const ClientCmd &cmd)
{
  QString command = cmd.command().toLower();
  if (command == LS("feeds")) {
    ClientCmd body(cmd.body());
    if (!body.isValid())
      ChatClient::feeds()->headers(dest);
    else if (body.command() == LS("local"))
      localFeeds(dest);

    return true;
  }

  if (command == LS("feed")) {
    ClientCmd body(cmd.body());
    if (!body.isValid())
      return true;

    if (body.command() == LS("revert")) {
      revert(dest, body);
    }
    else
      request(dest, body);

    return true;
  }

  return false;
}


/*!
 * Получение заголовков локальных фидов.
 */
void RawFeedsCmd::localFeeds(const QByteArray &dest)
{
  if (!TabWidget::i())
    return;

  ClientChannel channel = ChatClient::channels()->get(dest);
  if (!channel)
    return;

  QVariantMap json;

  QMapIterator<QString, FeedPtr> i(channel->feeds().all());
  while (i.hasNext()) {
    i.next();
    Feed::merge(i.key(), json, i.value()->head().save());
  }

  if (json.isEmpty())
    return;

  RawFeedsMessage message(dest, LS("headers"), Feed::merge(LS("feeds"), json));
  TabWidget::i()->add(message, false);
}


void RawFeedsCmd::request(const QByteArray &dest, const ClientCmd &cmd)
{
  ClientCmd body(cmd.body());
  if (body.command().isEmpty())
    return;

  QVariantMap json = JSON::parse(body.body().toUtf8()).toMap();
  ChatClient::feeds()->request(dest, cmd.command(), body.command(), json);
}


void RawFeedsCmd::revert(const QByteArray &dest, const ClientCmd &cmd)
{
  ClientCmd body(cmd.body());
  if (body.command().isEmpty())
    return;

  qint64 rev = body.body().toLongLong();
  ChatClient::feeds()->revert(dest, body.command(), rev);
}

} // namespace Hooks
