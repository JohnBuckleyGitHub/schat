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

#ifndef NODECHANNELINDEX_H_
#define NODECHANNELINDEX_H_

#include <QObject>

#include <ServerChannel.h>

class NotifyItem;

/*!
 * Информация о канале в списке каналов.
 */
class ChannelIndexData
{
public:
  ChannelIndexData(ChatChannel channel);
  bool isValid() const;
  bool operator<(const ChannelIndexData &other) const;

  int count;     ///< Число пользователей в канале.
  QString name;  ///< Имя канала.
  QString title; ///< Заголовок канала.
};


/*!
 * Формирует сортированный список каналов доступных на сервере.
 */
class NodeChannelIndex : public QObject
{
  Q_OBJECT

public:
  NodeChannelIndex(QObject *parent = 0);

private slots:
  void build();
  void notify(const NotifyItem &notify);

private:
  QList<ChatChannel> channels() const;

  qint64 m_date;                  ///< Дата формирования списка.
  QList<ChannelIndexData> m_list; ///< Список каналов.
};

#endif /* NODECHANNELINDEX_H_ */
