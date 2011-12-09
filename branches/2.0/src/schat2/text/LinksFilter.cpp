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
  : AbstractFilter("Links")
{
  m_scheme += "http://";
  m_scheme += "https://";
  m_scheme += "ftp://";
  m_scheme += "mailto:";
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
  }

  tokens = out;
  return false;
}


QString LinksFilter::url(const QString &text, int index, int &last) const
{
  last = text.indexOf(' ', index);
  QString url;
  last == -1 ? url = text.mid(index) : url = text.mid(index, last - index);
  return url;
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


/*!
 * Поиск ссылок в тексте и автоматическое преобразование их в html ссылки.
 */
void LinksFilter::parse(QList<HtmlToken> &tokens, const QString &text) const
{
  int index = -1;
  int last = -1;
  QString url;

  /// - http/https/ftp полный список в \p m_scheme.
  for (int i = 0; i < m_scheme.size(); ++i) {
    index = text.indexOf(m_scheme.at(i));
    if (index != -1) {
      if (index > 0)
        tokens.append(HtmlToken(text.left(index)));

      url = this->url(text, index, last);
      makeUrl(tokens, url, url);

      if (last != -1)
        return parse(tokens, text.mid(last));

      return;
    }
  }


  /// - Ссылки вида www.exampe.com в преобразуются в http.
  index = text.indexOf("www.");
  if (index != -1) {
    url = this->url(text, index, last);

    if (url.count('.') > 1) {
      if (index > 0)
        tokens.append(HtmlToken(text.left(index)));

      makeUrl(tokens, "http://" + url, url);

      if (last != -1)
        return parse(tokens, text.mid(last));

      return;
    }

    if (last != -1) {
      tokens.append(HtmlToken(text.left(last)));
      return parse(tokens, text.mid(last));
    }
  }

  /// - Ссылки вида user@example.com преобразуются в mailto.
  index = text.indexOf("@");
  if (index != -1) {
    int start = text.lastIndexOf(' ', index);
    QString name = text.mid(start + 1, index - start - 1);
    last = -1;

    if (!name.isEmpty()) {
      url = this->url(text, index, last);
      if (url.contains('.')) {
        if (index > 0) {
          tokens.append(HtmlToken(text.left(index - name.size())));

          makeUrl(tokens, "mailto:" + name + url, name + url);

          if (last != -1)
            return parse(tokens, text.mid(last));

          return;
        }
      }
    }

    if (last != -1) {
      tokens.append(HtmlToken(text.left(last)));
      return parse(tokens, text.mid(last));
    }
  }

  tokens.append(HtmlToken(text));
}