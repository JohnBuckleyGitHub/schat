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

#include "acl/Acl.h"
#include "schat.h"

/*!
 * Заголовок фида.
 */
class SCHAT_EXPORT FeedHeader
{
public:
  FeedHeader();
  bool setName(const QString &name);
  inline Acl& acl()                  { return m_acl; }
  inline const Acl& acl() const      { return m_acl; }
  QString toString() const;

  Acl m_acl;    ///< Права доступа к фиду.
  qint64 time;  ///< Время последнего обновления фида.
  QString name; ///< Имя фида.
};


/*!
 * Базовый класс для фидов.
 */
class SCHAT_EXPORT Feed
{
public:
  Feed();
  Feed(const QString &name);
  virtual ~Feed() {}

  virtual bool isValid() const;

  inline const FeedHeader& header() const  { return m_header; }
  inline const QByteArray& id() const      { return m_id; }
  inline const QString& name() const       { return m_header.name; }
  inline const QVariantMap& data() const   { return m_data; }
  inline FeedHeader& header()              { return m_header; }
  inline qint64 time() const               { return m_header.time; }

  inline bool setName(const QString &name) { return m_header.setName(name); }
  inline void setId(const QByteArray &id)  { m_id = id; }

private:
  FeedHeader m_header;  ///< Заголовок фида.
  QByteArray m_id;      ///< Идентификатор канала фида.
  QVariantMap m_data;   ///< JSON данные фида.
};

typedef QSharedPointer<Feed> FeedPtr;

#endif /* FEED_H_ */
