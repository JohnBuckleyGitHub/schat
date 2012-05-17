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

#ifndef TOKENFILTER_H_
#define TOKENFILTER_H_

#include <QHash>
#include <QStringList>
#include <QVariant>
#include <QSharedPointer>

#include "schat.h"
#include "text/HtmlToken.h"

/*!
 * Базовый абстрактный класс фильтрации текста.
 */
class SCHAT_CORE_EXPORT AbstractFilter
{
public:
  inline AbstractFilter()
  {}

  inline AbstractFilter(const QString &name)
  : m_name(name)
  {}

  virtual ~AbstractFilter() {}
  inline const QString& name() const { return m_name; }
  virtual bool filter(QList<HtmlToken> &tokens, QVariantHash options = QVariantHash()) const = 0;

protected:
  QString m_name;
};


typedef QSharedPointer<AbstractFilter> FilterPtr;


class SCHAT_CORE_EXPORT TokenFilter
{
  TokenFilter() {}

public:
  static QString filter(const QString &type, const QString &text);
  static void add(const QString &type, AbstractFilter *filter);

private:
  static QHash<QString, FilterPtr> m_filters; ///< Доступные фильтры.
};


#endif /* TOKENFILTER_H_ */
