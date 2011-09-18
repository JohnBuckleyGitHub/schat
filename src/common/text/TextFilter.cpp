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

#include "text/TextFilter.h"

QHash<QString, AbstractFilter *> TextFilter::m_filters;
TextFilter *TextFilter::m_self = 0;

TextFilter::TextFilter(QObject *parent)
  : QObject(parent)
{
  m_self = this;
}


bool TextFilter::add(AbstractFilter *filter)
{
  if (filter->name().isEmpty())
    return false;

  if (m_filters.contains(filter->name()))
    return false;

  m_filters[filter->name()] = filter;
  return true;
}


QString TextFilter::filter(const QString &name, const QString &text, QVariantHash options)
{
  if (m_filters.contains(name))
    return m_filters.value(name)->filter(text, options);

  return text;
}


void TextFilter::removeAll()
{
  qDeleteAll(m_filters);
  m_filters.clear();
}
