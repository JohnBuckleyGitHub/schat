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

#ifndef FEED_H_
#define FEED_H_

#include <QSharedPointer>
#include <QVariant>

#include "schat.h"

/*!
 * Базовый класс для фидов.
 */
class SCHAT_EXPORT Feed
{
public:
  Feed();
  virtual ~Feed() {}

  inline const QByteArray& channel() const { return m_channel; }
  inline const QString& name() const       { return m_name; }
  inline const QVariantMap& data() const   { return m_data; }
  inline qint64 time() const               { return m_time; }

private:
  QByteArray m_channel; ///< Идентификатор канала фида.
  qint64 m_time;        ///< Время последнего обновления фида.
  QString m_name;       ///< Имя фида.
  QVariantMap m_data;   ///< JSON данные фида.
};

typedef QSharedPointer<Feed> FeedPtr;

#endif /* FEED_H_ */
