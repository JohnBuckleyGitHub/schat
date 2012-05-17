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

#include <QDebug>

#include <QUrl>

#include "Emoticons.h"
#include "EmoticonsFilter.h"
#include "sglobal.h"

EmoticonsFilter::EmoticonsFilter(Emoticons *emoticons)
  : AbstractFilter(1000)
  , m_emoticons(emoticons)
{
}


bool EmoticonsFilter::filter(QList<HtmlToken> &tokens, QVariantHash /*options*/) const
{
  QList<HtmlToken> out;

  for (int i = 0; i < tokens.size(); ++i) {
    HtmlToken token = tokens.at(i);
    if (token.type == HtmlToken::Text && token.parent != LS("a"))
      parse(out, token.text);
    else
      out.append(token);
  }

  tokens = out;
  return false;
}


void EmoticonsFilter::make(QList<HtmlToken> &tokens, const QString &text) const
{
  Emoticon emoticon = m_emoticons->get(text);
  if (!emoticon) {
    tokens.append(HtmlToken(text));
    return;
  }

  HtmlToken a(HtmlToken::Tag, HtmlATag(LS("#"), text).toText());
  tokens.append(a);

  QString img = QString(LS("<img class=\"emoticon\" title=\"%1\" alt=\"%1\" src=\"%2\" width=\"%3\" height=\"%4\" /></a>"))
      .arg(text)
      .arg(QUrl::fromLocalFile(emoticon->file()).toString())
      .arg(emoticon->width())
      .arg(emoticon->height());

  HtmlToken tag(img);
  tag.parent = LS("a");
  tokens.append(tag);
  tokens.append(a.toEndTag());
}


void EmoticonsFilter::parse(QList<HtmlToken> &tokens, const QString &text, int pos) const
{
  if (text.isEmpty())
    return;

  if (pos == -1) {
    tokens.append(HtmlToken(text));
    return;
  }

  if (text.at(pos) == LC(' ') && pos < text.size())
    pos++;

  // Возможно в этой позиции находится начало смайла.
  if (m_emoticons->index().contains(text.at(pos))) {
    QString t = m_emoticons->find(text, pos);

    // Если текст не пустой, смайл найден.
    if (!t.isEmpty()) {

      // Смайл находится в конце строки.
      if (pos + t.size() == text.size()) {
        if (pos)
          tokens.append(HtmlToken(text.left(pos)));

        make(tokens, t);
        return;
      }
      // Смайл находится в внутри строки и содержит после себя пробел.
      else if (text.at(pos + t.size()) == LC(' ')) {
        if (pos)
          tokens.append(HtmlToken(text.left(pos)));

        make(tokens, t);
        parse(tokens, text.mid(pos + t.size()));
        return;
      }
    }
  }

  parse(tokens, text, text.indexOf(LC(' '), pos + 1));
}
