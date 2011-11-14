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
#include "User.h"

/*!
 * Заголовок фида.
 */
class SCHAT_EXPORT FeedHeader
{
public:
  FeedHeader();
  bool isValid() const;
  bool json(QVariantMap &out, ClientUser user = ClientUser()) const;
  inline Acl& acl() { return m_acl; }
  inline const Acl& acl() const { return m_acl; }
  inline const QByteArray& id() const { return m_id; }
  inline const QString& name() const { return m_name; }
  inline qint64 date() const { return m_date; }
  QVariantMap json(ClientUser user = ClientUser()) const;
  static qint64 timestamp();

  inline void setDate(qint64 date) { m_date = date; }
  inline void setId(const QByteArray &id) { m_id = id; }
  inline void setName(const QString &name) { m_name = name; }

private:
  Acl m_acl;         ///< Права доступа к фиду.
  QByteArray m_id;   ///< Идентификатор канала фида.
  qint64 m_date;     ///< Время последнего обновления фида.
  QString m_name;    ///< Имя фида.
};


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


/*!
 * Хранилище фидов.
 */
class SCHAT_EXPORT Feeds
{
public:
  Feeds() {}
  bool add(FeedPtr feed);
  inline bool add(Feed *feed) { return add(FeedPtr(feed)); }
  inline const QByteArray& id() const { return m_id; }
  inline const QHash<QString, FeedPtr>& all() const { return m_feeds; }
  inline void setId(const QByteArray &id) { m_id = id; }

  QVariantMap json(ClientUser user = ClientUser(), bool body = true);
  QVariantMap json(const QStringList &feeds, ClientUser user = ClientUser(), bool body = true);

  static bool merge(const QString &key, QVariantMap &out, const QVariantMap &in);
  static QVariantMap merge(const QString &key, const QVariantMap &in);

private:
  QByteArray m_id;                 ///< Идентификатор канала.
  QHash<QString, FeedPtr> m_feeds; ///< Таблица фидов.
};

#endif /* FEED_H_ */
