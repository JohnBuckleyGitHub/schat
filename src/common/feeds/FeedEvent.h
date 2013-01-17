/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#ifndef FEEDEVENT_H_
#define FEEDEVENT_H_

#include <QSharedPointer>

#include "schat.h"

/*!
 * Базовый класс событий фида.
 */
class SCHAT_EXPORT FeedEvent
{
public:
  FeedEvent();
  FeedEvent(const QByteArray &channel, const QByteArray &sender, const QString &method);
  bool isValid() const;

  int status;         ///< Код состояния операции над фидом. \sa Notice::StatusCodes.
  QByteArray channel; ///< Идентификатор канала фида.
  QByteArray sender;  ///< Идентификатор канала совершившего действие над фидом.
  qint64 date;        ///< Время модификации фида.
  QString method;     ///< Тип запроса.
  QString name;       ///< Имя фида.
  QString path;       ///< Путь запроса, может быть пустым.
};

typedef QSharedPointer<FeedEvent> FeedEventPtr;

#endif /* FEEDEVENT_H_ */
