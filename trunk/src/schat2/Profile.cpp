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
#include "client/ClientFeeds.h"
#include "Profile.h"
#include "sglobal.h"

QMap<int, QString> Profile::m_fields;
Profile *Profile::m_self = 0;

Profile::Profile(QObject *parent)
  : QObject(parent)
{
  if (m_self)
    add(this);
  else
    m_self = this;

  addField(LS("name"), 1000);
//         << LS("bday")
//         << LS("city")
//         << LS("country")
//         << LS("tel")
//         << LS("email")
//         << LS("site")
//         << LS("note");

  connect(ChatClient::i(), SIGNAL(ready()), SLOT(ready()));
}


/*!
 * Перевод имени поля.
 */
QString Profile::translate(const QString &field)
{
  if (field == LS("name"))
    return tr("Full name");

  QString result;
  foreach (Profile *hook, m_self->m_hooks) {
    result = hook->translateImpl(field);
    if (!result.isEmpty())
      return result;
  }

  return field;
}


/*!
 * Возвращает поля доступные для добавления.
 */
QStringList Profile::available()
{
  FeedPtr feed = ChatClient::channel()->feed(LS("profile"), false);
  if (!feed)
    return QStringList();

  QStringList filled = Profile::filled();
  QStringList out = fields();
  foreach (QString field, filled) {
    out.removeAll(field);
  }

  return out;
}


QStringList Profile::fields()
{
  return m_fields.values();
}


/*!
 * Получение списка заполненных полей.
 */
QStringList Profile::filled()
{
  FeedPtr feed = ChatClient::channel()->feed(LS("profile"), false);
  if (!feed)
    return QStringList();

  QStringList keys = feed->data().keys();
  keys.removeAll(LS("head"));
  if (keys.isEmpty())
    return QStringList();

  QStringList out;
  for (int i = 0; i < keys.size(); ++i) {
    QVariant data = feed->data().value(keys.at(i));
    if (data.type() == QVariant::String && data != QString())
      out.append(keys.at(i));

    else if (data.type() == QVariant::LongLong && data != 0)
      out.append(keys.at(i));

    else if (data.type() == QVariant::List && data != QVariantList())
      out.append(keys.at(i));

    else if (data.type() == QVariant::Map && data != QVariantMap())
      out.append(keys.at(i));
  }

  return out;
}


void Profile::addField(const QString &name, int weight)
{
  m_fields[weight] = name;
}


QString Profile::translateImpl(const QString & /*field*/) const
{
  return QString();
}


void Profile::ready()
{
  FeedPtr feed = ChatClient::channel()->feed(LS("profile"), false);
  if (!feed) {
    ChatClient::feeds()->request(ChatClient::id(), LS("add"), LS("profile"));
    ChatClient::feeds()->request(ChatClient::id(), LS("get"), LS("profile"));
  }
}
