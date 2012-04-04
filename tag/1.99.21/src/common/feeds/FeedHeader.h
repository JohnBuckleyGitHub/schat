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

#ifndef FEEDHEADER_H_
#define FEEDHEADER_H_

#include <QVariant>

#include "acl/Acl.h"
#include "schat.h"

/*!
 * Заголовок фида.
 */
class SCHAT_EXPORT FeedHeader
{
public:
  FeedHeader();
  bool isValid() const;
  inline Acl& acl()                        { return m_acl; }
  inline Channel *channel()                { return m_channel; }
  inline const Acl& acl() const            { return m_acl; }
  inline const QString& name() const       { return m_name; }
  inline const QVariantMap& data() const   { return m_data; }
  inline qint64 date() const               { return m_data.value(QLatin1String("date")).toLongLong(); }
  inline qint64 key() const                { return m_key; }
  inline qint64 rev() const                { return m_data.value(QLatin1String("rev")).toLongLong(); }
  inline QVariantMap& data()               { return m_data; }

  QVariantMap get(Channel *channel);
  QVariantMap save();

  inline void setChannel(Channel *channel) { m_channel = channel; }
  inline void setKey(qint64 key)           { m_key = key; }
  inline void setName(const QString &name) { m_name = name; }
  inline void setRev(qint64 rev)           { m_data[QLatin1String("rev")] = rev; }
  void setData(const QVariantMap &data);

private:
  Acl m_acl;          ///< Права доступа к фиду.
  Channel *m_channel; ///< Канал фидов.
  qint64 m_key;       ///< Ключ в базе данных.
  QString m_name;     ///< Имя фида.
  QVariantMap m_data; ///< Данные фида.
};

#endif /* FEEDHEADER_H_ */
