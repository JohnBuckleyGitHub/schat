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

#include "User.h"
#include "feeds/FeedHeader.h"

/*!
 * Базовый класс для фидов.
 */
class SCHAT_EXPORT Feed
{
public:
  Feed();
  Feed(const QString &name, qint64 date = 0);
  virtual ~Feed() {}

  virtual bool isValid() const;
  virtual QVariantMap json() const { return m_data; }

  inline const FeedHeader& h() const       { return m_header; }
  inline const QVariantMap& data() const   { return m_data; }
  inline FeedHeader& h()                   { return m_header; }

private:
  FeedHeader m_header;  ///< Заголовок фида.
  QVariantMap m_data;   ///< JSON данные фида.
};

typedef QSharedPointer<Feed> FeedPtr;

#endif /* FEED_H_ */
