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

#include "text/PlainTextFilter.h"

QString PlainTextFilter::filter(const QString &text)
{
  QString out = text;

  out.replace(QLatin1Char('\n'), "");
  out.replace(QLatin1String("</p>"), QLatin1String("\n"), Qt::CaseInsensitive);
  out.replace(QLatin1String("<br />"), QLatin1String("\n"), Qt::CaseInsensitive);

  removeTag(out, QLatin1String("style"));
  removeTag(out, QLatin1String("script"));

  int lt = 0;
  int gt = 0;
  forever {
    lt = out.indexOf(QLatin1Char('<'), lt);
    if (lt == -1)
      break;

    gt = out.indexOf(QLatin1Char('>'), lt);
    if (gt == -1) {
      out.remove(lt, out.size() - lt);
      break;
    }

    out.remove(lt, gt - lt + 1);
  }

  out.replace(QLatin1String("&gt;"),   QLatin1String(">"));
  out.replace(QLatin1String("&lt;"),   QLatin1String("<"));
  out.replace(QLatin1String("&quot;"), QLatin1String("\""));
  out.replace(QLatin1String("&nbsp;"), QLatin1String(" "));
  out.replace(QLatin1String("&amp;"),  QLatin1String("&"));
  out.replace(QChar(QChar::Nbsp),      QLatin1String(" "));
  out = out.trimmed();
  return out;
}


void PlainTextFilter::removeTag(QString &text, const QString &tag)
{
  int lt = 0;
  int gt = 0;
  forever {
    lt = text.indexOf(QLatin1String("<") + tag, 0, Qt::CaseInsensitive);
    if (lt == -1)
      break;

    gt = text.indexOf(QLatin1String("</") + tag + QLatin1String(">"), lt, Qt::CaseInsensitive);
    text.remove(lt, gt - lt + tag.size() + 3);
  }
}
