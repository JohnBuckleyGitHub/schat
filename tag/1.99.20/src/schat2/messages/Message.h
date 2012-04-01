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

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <QVariant>

#include "schat.h"

/*!
 * Базовый класс для текстовых сообщений.
 */
class SCHAT_CORE_EXPORT Message
{
public:
  Message();
  inline const QByteArray& tab() const   { return m_tab; }
  inline const QVariantMap& data() const { return m_data; };
  inline QVariantMap& data()             { return m_data; };
  inline virtual bool isValid() const    { return true; }

protected:
  void author(const QByteArray &id);

  QByteArray m_tab;   ///< Идентификатор вкладки в котором будет отображено сообщение.
  QVariantMap m_data; ///< JSON данные.
};

#endif /* MESSAGE_H_ */
