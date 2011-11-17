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

#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <QList>
#include <QSharedPointer>
#include <QVariant>

#include "feeds/Feed.h"
#include "Gender.h"

/*!
 * \deprecated Этот класс является устаревшим, в связи с заменой топика на фид.
 */
class Topic
{
public:
  Topic()
  : timestamp(0)
  {}

  Topic(const QByteArray &channel, const QByteArray &author, const QString &topic, qint64 timestamp)
  : author(author)
  , channel(channel)
  , timestamp(timestamp)
  , topic(topic)
  {}

  QByteArray author;
  QByteArray channel;
  qint64 timestamp;
  QString topic;
};


/*!
 * Базовый статус пользователя.
 */
class Status
{
public:
  /// Статус.
  enum Statuses {
    Offline,
    Online,
    Away,
    AutoAway,
    DnD,
    FreeForChat
  };

  Status(quint8 status = Offline)
  : m_status(status)
  {}

  inline quint8 value() const    { return m_status; }
  inline void set(quint8 status) { m_status = status; }

private:
  quint8 m_status;
};


/*!
 * Список каналов.
 */
class SCHAT_EXPORT Channels
{
public:
  Channels() {}

  bool add(const QByteArray &id);
  inline const QList<QByteArray>& all() const        { return m_channels; }
  inline void clear()                                { m_channels.clear(); }
  inline void remove(const QByteArray &id)           { m_channels.removeAll(id); }
  inline void set(const QList<QByteArray> &channels) { m_channels = channels; }

private:
  QList<QByteArray> m_channels; ///< Список идентификаторов каналов-подписчиков.
};


/*!
 * Базовый класс канала.
 */
class SCHAT_EXPORT Channel
{
public:
  /// Ограничения.
  enum Limits {
    MinNameLengh = 3,     ///< Минимальная длина имени.
    MaxNameLength = 20,   ///< Максимальная длина имени.
  };

  Channel();
  Channel(const QByteArray &id, const QString &name);
  virtual ~Channel() {}
  virtual bool isValid() const;

  inline bool isSynced() const { return m_synced; }
  inline const QString& name() const { return m_name; }
  inline int type() const { return m_type; }
  inline QByteArray id() const { return m_id; }

  bool setId(const QByteArray &id);
  bool setName(const QString &name);
  inline void setSynced(bool synced) { m_synced = synced; }

  inline Channels& channels() { return m_channels; }
  inline const Channels& channels() const { return m_channels; }
  inline const Feeds& feeds() const { return m_feeds; }
  inline const Gender& gender() const { return m_gender; }
  inline const Status& status() const { return m_status; }
  inline Feeds& feeds() { return m_feeds; }
  inline Gender& gender() { return m_gender; }
  inline Status& status() { return m_status; }

  static int isCompatibleId(const QByteArray &id);

private:
  bool m_synced;                   ///< true если канал синхронизирован.
  Channels m_channels;             ///< Список каналов.
  Feeds m_feeds;                   ///< Таблица фидов.
  Gender m_gender;                 ///< Пол и цвет иконки.
  int m_type;                      ///< Тип канала, соответствует типу идентификатора канала \sa SimpleID::Types.
  QByteArray m_id;                 ///< Уникальный идентификатор канала.
  QString m_name;                  ///< Имя канала.
  Status m_status;                 ///< Статус пользователя.
};

typedef QSharedPointer<Channel> ClientChannel;

#endif /* CHANNEL_H_ */
