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

#ifndef TOKENFILTER_H_
#define TOKENFILTER_H_

#include <QHash>
#include <QStringList>
#include <QVariant>

#include "schat.h"
#include "text/HtmlToken.h"

/*!
 * Базовый абстрактный класс фильтрации текста.
 */
class AbstractFilter
{
public:
  inline AbstractFilter()
  {}

  inline AbstractFilter(const QString &name)
  : m_name(name)
  {}

  virtual ~AbstractFilter() {}
  inline QString name() const { return m_name; }
  virtual bool filter(QList<HtmlToken> &tokens, QVariantHash options = QVariantHash()) const = 0;

protected:
  QString m_name;
};


class SCHAT_CORE_EXPORT TokenFilter
{
  TokenFilter() {}

public:
  static bool add(AbstractFilter *filter);
  static bool add(const QString &type, AbstractFilter *filter);
  static bool add(const QString &type, const QString &filter);
  static bool filter(const QString &name, QList<HtmlToken> &tokens, QVariantHash options = QVariantHash());
  static QStringList defaults(const QString &name);
  static void remove(const QString &name);
  static void removeAll();

private:
  static QHash<QString, AbstractFilter *> m_filters; ///< Доступные фильтры.
  static QHash<QString, QStringList> m_default;      ///< Конфигурация фильтров по умолчанию.
};


#endif /* TOKENFILTER_H_ */
