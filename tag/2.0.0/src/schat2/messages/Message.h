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

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <QVariant>

#include "schat.h"

#define MESSAGE_AUTHOR QLatin1String("Author")
#define MESSAGE_DATE   QLatin1String("Date")
#define MESSAGE_EXTRA  QLatin1String("Extra")
#define MESSAGE_FUNC   QLatin1String("Func")
#define MESSAGE_ID     QLatin1String("Id")
#define MESSAGE_TEXT   QLatin1String("Text")
#define MESSAGE_TYPE   QLatin1String("Type")

/*!
 * Базовый класс для текстовых сообщений.
 */
class SCHAT_CORE_EXPORT Message
{
public:
  Message();
  Message(const QByteArray &id, const QByteArray &tab = QByteArray(), const QString &type = QString(), const QString &func = QString());
  virtual ~Message() {}

  inline const QByteArray& tab() const   { return m_tab; }
  inline const QVariantMap& data() const { return m_data; };
  inline QString id() const              { return m_data.value(MESSAGE_ID).toString(); }
  inline QVariantMap& data()             { return m_data; };
  inline virtual bool isValid() const    { return true; }
  static QByteArray detectTab(const QByteArray &sender, const QByteArray &dest);
  void setAuthor(const QByteArray &id);
  void setDate(qint64 date = 0);
  void setId(const QByteArray &id);

protected:
  QByteArray m_tab;   ///< Идентификатор вкладки в котором будет отображено сообщение.
  QVariantMap m_data; ///< JSON данные.
};

#endif /* MESSAGE_H_ */