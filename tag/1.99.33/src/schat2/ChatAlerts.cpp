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

#include "ChatAlerts.h"
#include "ChatCore.h"
#include "client/ChatClient.h"
#include "client/SimpleClient.h"
#include "DateTime.h"
#include "messages/Message.h"
#include "sglobal.h"

ChatAlerts *ChatAlerts::m_self = 0;
QList<QByteArray> ChatAlerts::m_channels;

Alert::Alert(const QString &type, Options options)
  : m_options(options)
  , m_id(ChatCore::randomId())
  , m_date(DateTime::utc())
  , m_type(type)
{
}


Alert::Alert(const QString &type, const QByteArray &id, qint64 date, Options options)
  : m_options(options)
  , m_id(id)
  , m_date(date)
  , m_type(type)
{
}


Alert::Alert(const QString &type, qint64 date, Options options)
  : m_options(options)
  , m_id(ChatCore::randomId())
  , m_date(date)
  , m_type(type)
{
}


void Alert::setTab(const QByteArray &sender, const QByteArray &dest)
{
  setTab(Message::detectTab(sender, dest));
}


ChatAlerts::ChatAlerts(QObject *parent)
  : QObject(parent)
{
  m_self = this;

  connect(ChatClient::i(), SIGNAL(offline()), SLOT(offline()));
  connect(ChatClient::i(), SIGNAL(online()), SLOT(online()));
}


void ChatAlerts::add(const QByteArray &id)
{
  if (m_channels.contains(id))
    return;

  m_channels.append(id);
  if (m_channels.size() == 1)
    emit m_self->alert(true);
}


void ChatAlerts::remove(const QByteArray &id)
{
  m_channels.removeAll(id);

  if (m_channels.isEmpty())
    emit m_self->alert(false);
}


void ChatAlerts::start(const Alert &alert)
{
  emit m_self->alert(alert);
}


void ChatAlerts::offline()
{
  Alert alert(LS("offline"));
  start(alert);
}


void ChatAlerts::online()
{
  Alert alert(LS("online"), ChatClient::io()->date());
  start(alert);
}
