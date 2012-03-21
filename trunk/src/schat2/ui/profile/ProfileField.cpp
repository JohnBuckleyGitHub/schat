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

#include <QLabel>
#include <QTimer>

#include "client/ChatClient.h"
#include "client/ClientFeeds.h"
#include "Profile.h"
#include "sglobal.h"
#include "ui/profile/ProfileField.h"

ProfileField::ProfileField(const QString &field, QWidget *parent)
  : QWidget(parent)
  , m_field(field)
{
  m_label = new QLabel(Profile::translate(field) + LC(':'), this);

  QTimer::singleShot(0, this, SLOT(reload()));
}


void ProfileField::reload()
{
  FeedPtr feed = ChatClient::channel()->feed(LS("profile"), false);
  if (!feed)
    return;

  QVariant data = feed->data().value(m_field);
  if (data.isNull())
    return;

  setData(data);
}


void ProfileField::setData(const QVariant &)
{
}


void ProfileField::apply(const QVariant &value)
{
  FeedPtr feed = ChatClient::channel()->feed(LS("profile"), false);
  if (!feed)
    return;

  if (feed->data().value(m_field) == value)
    return;

  QVariantMap query;
  query[m_field] = value;
  ChatClient::feeds()->query(LS("profile"), LS("x-set"), query);
}
