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

#ifndef MESSAGEID_H_
#define MESSAGEID_H_

#include <QString>

#include "schat.h"

/*!
 * Идентификатор сообщения.
 */
class SCHAT_EXPORT MessageId
{
public:
  MessageId(const QString &id);
  MessageId(qint64 date, const QByteArray &id);
  bool isValid() const;
  inline const QByteArray& id() const { return m_id; }
  inline qint64 date() const          { return m_date; }
  QString toString() const;

  static QList<MessageId> toList(const QString &ids);
  static QString toString(const QList<MessageId> &ids);

private:
  QByteArray m_id; ///< Идентификатор сообщения.
  qint64 m_date;   ///< Время сообщения.
};

#endif /* MESSAGEID_H_ */
