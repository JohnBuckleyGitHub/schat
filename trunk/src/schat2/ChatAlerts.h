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

#ifndef CHATALERTS_H_
#define CHATALERTS_H_

#include <QObject>
#include <QVariant>

#include "schat.h"

class SCHAT_CORE_EXPORT Alert
{
public:
  Alert(int type);
  virtual ~Alert() {}

  inline const QByteArray id() const     { return m_id; }
  inline const QByteArray& tab() const   { return m_tab; }
  inline const QVariantMap& data() const { return m_data; }
  inline int type() const                { return m_type; }
  inline qint64 date() const             { return m_date; }

  inline QVariantMap& data()             { return m_data; }

protected:
  int m_type;          ///< Тип оповещения.
  QByteArray m_id;     ///< Уникальный идентификатор оповещения.
  QByteArray m_tab;    ///< Идентификатор вкладки канала.
  qint64 m_date;       ///< Отметка времени.
  QVariantMap m_data;  ///< JSON данные.
};


class SCHAT_CORE_EXPORT ChatAlerts : public QObject
{
  Q_OBJECT

public:
  ChatAlerts(QObject *parent = 0);
  inline static ChatAlerts *i() { return m_self; }

private:
  static ChatAlerts *m_self; ///< Указатель на себя.
};

#endif /* CHATALERTS_H_ */
