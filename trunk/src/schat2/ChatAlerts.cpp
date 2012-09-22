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

#include "alerts/AlertType.h"
#include "alerts/MessageAlert.h"
#include "ChatAlerts.h"
#include "ChatCore.h"
#include "client/ChatClient.h"
#include "client/SimpleClient.h"
#include "DateTime.h"
#include "messages/Message.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "ui/TabWidget.h"

ChatAlerts *ChatAlerts::m_self = 0;
int ChatAlerts::m_alerts = 0;
QHash<QByteArray, int> ChatAlerts::m_count;
QList<QByteArray> ChatAlerts::m_channels;

Alert::Alert(const QString &type)
  : m_id(ChatCore::randomId())
  , m_date(DateTime::utc())
  , m_type(type)
{
}


Alert::Alert(const QString &type, const QByteArray &id, qint64 date)
  : m_id(id)
  , m_date(date)
  , m_type(type)
{
}


Alert::Alert(const QString &type, qint64 date)
  : m_id(ChatCore::randomId())
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

  add(new MessageAlertType(LS("public"),  100));
  add(new MessageAlertType(LS("private"), 200));

  connect(ChatClient::i(), SIGNAL(offline()), SLOT(offline()));
  connect(ChatClient::i(), SIGNAL(online()), SLOT(online()));
}


ChatAlerts::~ChatAlerts()
{
  qDeleteAll(m_types);
}


bool ChatAlerts::add(AlertType *type)
{
  if (!type || m_self->m_types.contains(type->type()))
    return false;

  m_self->m_types[type->type()] = type;
  return true;
}


/*!
 * Запуск уведомления.
 */
bool ChatAlerts::start(const Alert &alert)
{
  AlertType *type = m_self->m_types.value(alert.type());
  if (!type)
    return false;

  if (type->tray()) {
    const QByteArray &tabId = alert.tab();
    if (TabWidget::isActive(tabId))
      return false;

    int idType = SimpleID::typeOf(tabId);
    if (idType == SimpleID::ChannelId || idType == SimpleID::UserId) {
      ChannelBaseTab *tab = TabWidget::i()->channelTab(tabId, false, false);
      if (!tab)
        return false;

      add(tabId);
    }
    else
      return false;
  }

  emit m_self->alert(alert);
  return true;
}


/*!
 * Удаление глобального уведомления для канала.
 */
void ChatAlerts::remove(const QByteArray &id)
{
  int count = m_count.value(id);
  m_channels.removeAll(id);
  m_count.remove(id);
  m_alerts -= count;

  if (m_channels.isEmpty())
    emit m_self->alert(false);

  emit m_self->countChanged(m_alerts, count, id);
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


/*!
 * Добавление глобального уведомления для канала.
 */
void ChatAlerts::add(const QByteArray &id)
{
  m_channels.removeAll(id);
  m_channels.prepend(id);

  int count = m_count.value(id);
  count++;
  m_alerts++;
  m_count[id] = count;

  if (m_channels.size() == 1)
    emit m_self->alert(true);

  emit m_self->countChanged(m_alerts, count, id);
}
