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

#ifndef NODENOTIFY_H_
#define NODENOTIFY_H_

#include <QObject>
#include <QQueue>
#include <QSharedPointer>
#include <QVariant>

#include "schat.h"

class NotifyItem
{
public:
  /// Типы уведомлений.
  enum Types {
    /// "CB" Изменено связывание каналов, \b subType: \b 1 канал был добавлен, \b 0 канал был удалён,
    /// \b param1: идентификатор канала, \b param2: идентификатор добавленного или удалённого канала.
    ChannelBonding = 0x4342,
    /// "FM" Уведомление об изменении фида.
    /// \b param1: идентификатор канала фида, \b param2: имя фида.
    FeedModified   = 0x464D
  };

  NotifyItem(int type, int subType = 0, const QVariant &param1 = QVariant(), const QVariant &param2 = QVariant())
  : m_subType(subType)
  , m_type(type)
  , m_param1(param1)
  , m_param2(param2)
  {}

  virtual ~NotifyItem() {}

  inline const QVariant& param1() const { return m_param1; }
  inline const QVariant& param2() const { return m_param2; }
  inline int subType() const            { return m_subType; }
  inline int type() const               { return m_type; }

protected:
  int m_subType;     ///< Дополнительный тип уведомления.
  int m_type;        ///< Тип уведомления.
  QVariant m_param1; ///< Данные уведомления.
  QVariant m_param2; ///< Данные уведомления.
};

typedef QSharedPointer<NotifyItem> NotifyItemPtr;


class SCHAT_EXPORT NodeNotify : public QObject
{
  Q_OBJECT

public:
  NodeNotify(QObject *parent = 0);
  inline static NodeNotify *i() { return m_self; }
  static void start(int type, int subType = 0, const QVariant &param1 = QVariant(), const QVariant &param2 = QVariant());
  static void start(NotifyItem *notify);

signals:
  void notify(const NotifyItem &notify);

private slots:
  void start();

private:
  QQueue<NotifyItemPtr> m_queue; ///< Очередь отправки.
  static NodeNotify *m_self;     ///< Указатель на себя.
};

#endif /* NODENOTIFY_H_ */
