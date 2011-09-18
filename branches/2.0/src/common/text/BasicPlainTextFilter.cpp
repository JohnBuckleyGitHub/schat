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

#include <QRegExp>

#include "text/BasicPlainTextFilter.h"

BasicPlainTextFilter::BasicPlainTextFilter()
  : AbstractFilter(QLatin1String("BasicPlainText"))
{
}


QString BasicPlainTextFilter::filter(const QString &text, QVariantHash options) const
{
  Q_UNUSED(options)
  QString out = text;

  out.replace(QLatin1String("<br />"), QLatin1String("\n"), Qt::CaseInsensitive);
  out.remove(QLatin1String("</span>"), Qt::CaseInsensitive);
  out.remove(QRegExp(QLatin1String("<style.*</style>")));
  out.remove(QRegExp(QLatin1String("<[^>]*>")));

  out.replace(QLatin1String("&gt;"),   QLatin1String(">"));
  out.replace(QLatin1String("&lt;"),   QLatin1String("<"));
  out.replace(QLatin1String("&quot;"), QLatin1String("\""));
  out.replace(QLatin1String("&nbsp;"), QLatin1String(" "));
  out.replace(QLatin1String("&amp;"),  QLatin1String("&"));
  out.replace(QChar(QChar::Nbsp),      QLatin1String(" "));
  out = out.trimmed();
  return out;
}
