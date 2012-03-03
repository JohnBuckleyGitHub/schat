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

#include "ChatAlerts.h"
#include "ChatCore.h"
#include "client/ChatClient.h"
#include "DateTime.h"
#include "client/SimpleClient.h"

ChatAlerts *ChatAlerts::m_self = 0;

Alert::Alert(int type)
  : m_type(type)
  , m_id(ChatCore::randomId())
  , m_date(DateTime::utc())
{
}


Alert::Alert(int type, const QByteArray &id, qint64 date)
  : m_type(type)
  , m_id(id)
  , m_date(date)
{
}

ChatAlerts::ChatAlerts(QObject *parent)
  : QObject(parent)
{
  m_self = this;

  connect(ChatClient::i(), SIGNAL(offline()), SLOT(offline()));
  connect(ChatClient::i(), SIGNAL(online()), SLOT(online()));
}


void ChatAlerts::offline()
{
  Alert alert(Alert::ConnectionLost);
  start(alert);
}


void ChatAlerts::online()
{
  Alert alert(Alert::Connected, ChatCore::randomId(), ChatClient::io()->date());
  start(alert);
}


void ChatAlerts::startAlert(const Alert &alert)
{
  emit this->alert(alert);
}


void ChatAlerts::addImpl(const QByteArray &id)
{
  if (!m_channels.contains(id)) {
    m_channels += id;

    if (m_channels.size() == 1)
      emit alert(true);
  }
}


void ChatAlerts::removeImpl(const QByteArray &id)
{
  m_channels.removeAll(id);

  if (m_channels.isEmpty())
    emit alert(false);
}
