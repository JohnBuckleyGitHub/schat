/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2014 IMPOMEZIA <schat@impomezia.com>
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
#include "client/ClientChannels.h"
#include "client/ClientCmd.h"
#include "client/ClientFeeds.h"
#include "client/ClientMessages.h"
#include "JSON.h"
#include "net/NetRequest.h"
#include "RawFeedsCmd.h"
#include "RawFeedsMessage.h"
#include "RawFeedsPlugin_p.h"
#include "sglobal.h"
#include "ui/TabWidget.h"

namespace Hooks
{

RawFeedsCmd::RawFeedsCmd(RawFeeds *parent)
  : Messages(parent)
  , m_plugin(parent)
{
  ChatClient::messages()->add(this);
}


bool RawFeedsCmd::command(const QByteArray &dest, const ClientCmd &cmd)
{
  QString command = cmd.command().toLower();
  if (command == LS("feeds")) {
    m_plugin->setEnabled(true);

    ClientCmd body(cmd.body());
    if (!body.isValid() && Channel::isCompatibleId(dest)) {
      QVariantMap json;
      json[FEED_KEY_COMPACT] = false;
      ClientFeeds::request(dest, FEED_METHOD_GET, FEED_WILDCARD_ASTERISK, json);
    }
    else if (body.command() == LS("local"))
      localFeeds(dest);
    else if (body.command() == LS("off"))
      m_plugin->setEnabled(false);

    return true;
  }

  if (command == LS("feed")) {
    m_plugin->setEnabled(true);

    ClientCmd body(cmd.body());
    if (!body.isValid())
      return true;

    request(dest, body);
    return true;
  }

  if (command == LS("headers")) {
    m_headers = JSON::parse(cmd.body().toUtf8()).toMap();
    return true;
  }

  if (command == "req") {
    ClientCmd body(cmd.body());
    request(body.command().toUpper(), dest, body.body());
    return true;
  }

  if (command == LS("auth") || command == LS("del") || command == LS("get") || command == LS("post") || command == LS("pub") || command == LS("put") || command == LS("sub") || command == LS("unsub")) {
    request(command.toUpper(), dest, cmd.body());
    return true;
  }

  return false;
}


/*!
 * Получение заголовков локальных фидов.
 */
void RawFeedsCmd::localFeeds(const QByteArray &dest)
{
  if (!m_plugin->isEnabled())
    return;

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
  TabWidget::add(message, false);
}


void RawFeedsCmd::request(const QByteArray &dest, const ClientCmd &cmd)
{
  ClientCmd body(cmd.body());
  if (body.command().isEmpty())
    return;

  QVariantMap json = JSON::parse(body.body().toUtf8()).toMap();
  ClientFeeds::request(ChatClient::channels()->get(dest), cmd.command(), body.command(), json);
}


void RawFeedsCmd::request(const QString &method, const QByteArray &dest, const QString &body)
{
  NetRequest req;
  req.method  = method;
  req.headers = m_headers;
  m_headers.clear();

  const int index = body.indexOf(LC(' '));
  if (index != -1) {
    req.request = body.left(index);
    req.data    = JSON::parse(body.mid(index + 1).toUtf8());
  }
  else
    req.request = body;

  if (req.request.startsWith(LC('$')))
    req.request.replace(0, 1, ChatId::toBase32(dest));

  m_plugin->add(req.id);
  ClientFeeds::req(req);
}

} // namespace Hooks
