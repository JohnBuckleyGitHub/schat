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

#include <QDebug>

#include "text/LinksFilter.h"

LinksFilter::LinksFilter()
  : AbstractFilter(QLatin1String("Links"))
{
}


bool LinksFilter::filter(QList<HtmlToken> &tokens, QVariantHash options) const
{
  qDebug() << "LinksFilter::filter()";

  QList<HtmlToken> out;

  for (int i = 0; i < tokens.size(); ++i) {
    HtmlToken token = tokens.at(i);
    if (token.type == HtmlToken::Text && token.parent != "a")
      parse(out, token.text);
    else
      out.append(token);

//    qDebug() << "--" << tokens.at(i).parent << tokens.at(i).text;
  }

  tokens = out;
  return false;
}


void LinksFilter::parse(QList<HtmlToken> &tokens, const QString &text) const
{
  int index = -1;
  QString url;

  index = text.indexOf("http://");
  if (index != -1) {
    if (index > 0)
      tokens.append(HtmlToken(text.left(index)));

    int last = text.indexOf(' ', index);
    if (last == -1) {
      url = text.mid(index);
      makeUrl(tokens, url, url);
      return;
    }
    else {
      url = text.mid(index, last - index);
      makeUrl(tokens, url, url);
      return parse(tokens, text.mid(last));
    }
  }


  tokens.append(HtmlToken(text));
}


void LinksFilter::makeUrl(QList<HtmlToken> &tokens, const QString &url, const QString &text) const
{
  HtmlToken a(HtmlToken::Tag, HtmlATag(url).toText());
  tokens.append(a);

  HtmlToken tag(text);
  tag.parent = "a";
  tokens.append(tag);

  tokens.append(a.toEndTag());
}
