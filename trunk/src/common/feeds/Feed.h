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

#ifndef FEED_H_
#define FEED_H_

#include <QSharedPointer>
#include <QVariant>

#include "feeds/FeedHeader.h"

class FeedQueryReply
{
public:
  FeedQueryReply(int status, const QVariantMap &json = QVariantMap())
  : broadcast(true)
  , modified(false)
  , single(false)
  , status(status)
  , json(json)
  {}

  bool broadcast;            ///< \b true если нужно разослать тело фида если оно изменилось.
  bool modified;             ///< \b true если тело фида изменилось.
  bool single;               ///< \b true если ответ должен получить только пользователь создавший запрос.
  int status;                ///< Статус операции.
  QList<QByteArray> packets; ///< Дополнительные пакеты с ответом на запрос.
  QVariantMap json;          ///< JSON ответ на запрос.
};

/*!
 * Базовый класс для фидов.
 */
class SCHAT_EXPORT Feed
{
public:
  Feed(const QString &name, const QVariantMap &data);
  Feed(const QString &name = QString(), qint64 date = 0);
  virtual ~Feed() {}

  virtual bool isValid() const;
  virtual Feed* create(const QString &name);
  virtual Feed* load(const QString &name, const QVariantMap &data);
  virtual FeedQueryReply query(const QVariantMap &json, Channel *channel = 0);
  virtual int clear(Channel *channel = 0);
  virtual int update(const QVariantMap &json, Channel *channel = 0);
  virtual QVariantMap feed(Channel *channel = 0);
  virtual QVariantMap save();

  virtual void setChannel(Channel *channel);

  bool canEdit(Channel *channel) const;
  bool canRead(Channel *channel) const;
  bool canWrite(Channel *channel) const;
  inline const FeedHeader& head() const    { return m_header; }
  inline const QVariantMap& data() const   { return m_data; }
  inline FeedHeader& head()                { return m_header; }

  static bool merge(const QString &key, QVariantMap &out, const QVariantMap &in);
  static QVariantMap merge(const QString &key, const QVariantMap &in);
  static void merge(QVariantMap &out, const QVariantMap &in);

protected:
  FeedHeader m_header;  ///< Заголовок фида.
  QVariantMap m_data;   ///< JSON данные фида.

private:
  FeedQueryReply mask(const QVariantMap &json, Channel *channel);
  FeedQueryReply set(const QVariantMap &json, Channel *channel);
};

typedef QSharedPointer<Feed> FeedPtr;

#endif /* FEED_H_ */
