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

#include "text/TokenFilter.h"

QHash<QString, AbstractFilter *> TokenFilter::m_filters;
QHash<QString, QStringList> TokenFilter::m_default;


/*!
 * Добавление нового фильтра.
 */
bool TokenFilter::add(AbstractFilter *filter)
{
  if (filter->name().isEmpty() || m_filters.contains(filter->name())) {
    delete filter;
    return false;
  }

  m_filters[filter->name()] = filter;
  return true;
}


bool TokenFilter::add(const QString &type, AbstractFilter *filter)
{
  if (add(filter))
    return add(type, filter->name());

  return false;
}


bool TokenFilter::add(const QString &type, const QString &filter)
{
  if (m_default.contains(type) && m_default.value(type).contains(filter))
    return false;

  m_default[type].append(filter);
  return true;
}


bool TokenFilter::filter(const QString &name, QList<HtmlToken> &tokens, QVariantHash options)
{
  if (m_filters.contains(name))
    return m_filters.value(name)->filter(tokens, options);

  return false;
}


QStringList TokenFilter::defaults(const QString &name)
{
  return m_default.value(name);
}


/*!
 * Удаление фильтра.
 */
void TokenFilter::remove(const QString &name)
{
  m_filters.remove(name);
}


void TokenFilter::removeAll()
{
  qDeleteAll(m_filters);
  m_filters.clear();
}
